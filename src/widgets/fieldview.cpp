#include "fieldview.h"

#include <QColorDialog>
#include <QMenu>

// commands
#include "undo/deletefieldcommand.h"
#include "undo/setfieldproperties.h"

// Delegates
#include "borderlisteditor.h"
#include "delegate/alignmentdelegate.h"
#include "delegate/fontdelegate.h"
#include "delegate/pagedelegate.h"
#include "delegate/typedelegate.h"

#include "dialog/codeeditordialog.h"

FieldView::FieldView(QWidget* parent) : QTreeView(parent), m_mapper(new QSignalMapper(this))
{
    // Item action
    m_delItem= new QAction(tr("Delete Item"), this);
    m_applyValueOnSelection= new QAction(tr("Apply on Selection"), this);
    m_applyValueOnAllLines= new QAction(tr("Apply on all lines"), this);
    m_defineCode= new QAction(tr("Define Field Code"), this);
    m_resetCode= new QAction(tr("Reset Field Code"), this);

    m_showGeometryGroup= new QAction(tr("Position columns"), this);
    connect(m_showGeometryGroup, &QAction::triggered, this, [=]() {
        hideAllColumns(true);
        showColumn(CharacterSheetItem::ID);
        showColumn(CharacterSheetItem::LABEL);
        showColumn(CharacterSheetItem::X);
        showColumn(CharacterSheetItem::Y);
        showColumn(CharacterSheetItem::WIDTH);
        showColumn(CharacterSheetItem::HEIGHT);
        showColumn(CharacterSheetItem::PAGE);
    });
    m_showEsteticGroup= new QAction(tr("Esthetic columns"), this);
    connect(m_showEsteticGroup, &QAction::triggered, this, [=]() {
        hideAllColumns(true);
        showColumn(CharacterSheetItem::ID);
        showColumn(CharacterSheetItem::LABEL);
        showColumn(CharacterSheetItem::BGCOLOR);
        showColumn(CharacterSheetItem::BORDER);
        showColumn(CharacterSheetItem::CLIPPED);
        showColumn(CharacterSheetItem::FONT);
        showColumn(CharacterSheetItem::TEXT_ALIGN);
        showColumn(CharacterSheetItem::TEXTCOLOR);
    });
    m_showValueGroup= new QAction(tr("Value columns"), this);
    connect(m_showValueGroup, &QAction::triggered, this, [=]() {
        hideAllColumns(true);
        showColumn(CharacterSheetItem::ID);
        showColumn(CharacterSheetItem::LABEL);
        showColumn(CharacterSheetItem::VALUE);
        showColumn(CharacterSheetItem::VALUES);
        showColumn(CharacterSheetItem::TYPE);
    });
    m_showIdGroup= new QAction(tr("Id columns"), this);
    connect(m_showIdGroup, &QAction::triggered, this, [=]() {
        hideAllColumns(true);
        showColumn(CharacterSheetItem::ID);
        showColumn(CharacterSheetItem::LABEL);
    });

    m_showAllGroup= new QAction(tr("All columns"), this);
    connect(m_showAllGroup, &QAction::triggered, this, [=]() { hideAllColumns(false); });

    setAlternatingRowColors(true);
#ifdef Q_OS_MACX
    setAlternatingRowColors(false);
#endif

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editColor(QModelIndex)));

    AlignmentDelegate* delegate= new AlignmentDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::TEXT_ALIGN), delegate);

    TypeDelegate* typeDelegate= new TypeDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::TYPE), typeDelegate);

    FontDelegate* fontDelegate= new FontDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::FONT), fontDelegate);

    PageDelegate* pageDelegate= new PageDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::PAGE), pageDelegate);

    setItemDelegateForColumn(CharacterSheetItem::BORDER, new BorderListEditor);

    connect(m_mapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this,
            [this](int i) { this->setColumnHidden(i, !this->isColumnHidden(i)); });
}
void FieldView::hideAllColumns(bool hidden)
{
    if(m_model)
    {
        auto columCount= m_model->columnCount(QModelIndex());
        for(int i= 0; i < columCount; ++i)
        {
            this->setColumnHidden(i, hidden);
        }
    }
}
void FieldView::contextMenuEvent(QContextMenuEvent* event)
{

    QMenu menu(this);

    QModelIndex index= currentIndex();
    if(index.isValid())
    {

        menu.addAction(m_applyValueOnSelection);
        menu.addAction(m_applyValueOnAllLines);
        menu.addSeparator();
        menu.addAction(m_defineCode);
        if(nullptr != m_canvasList)
        {
            menu.addSeparator();
            menu.addAction(m_delItem);
        }
    }
    auto showSubMenu= menu.addMenu(tr("Show"));
    showSubMenu->addAction(m_showAllGroup);
    showSubMenu->addAction(m_showEsteticGroup);
    showSubMenu->addAction(m_showIdGroup);
    showSubMenu->addAction(m_showValueGroup);
    showSubMenu->addAction(m_showGeometryGroup);

    auto hideSubMenu= menu.addMenu(tr("Show/Hide"));
    auto columnCount= m_model->columnCount(QModelIndex());
    for(int i= 0; i < columnCount; ++i)
    {
        auto name= m_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        auto act= hideSubMenu->addAction(name);
        act->setCheckable(true);
        act->setChecked(!isColumnHidden(i));
        connect(act, SIGNAL(triggered(bool)), m_mapper, SLOT(map()));
        m_mapper->setMapping(act, i);
    }

    QAction* act= menu.exec(event->globalPos());

    if(act == m_delItem)
    {
        auto itemData= static_cast<Field*>(index.internalPointer());
        DeleteFieldCommand* deleteCommand
            = new DeleteFieldCommand(itemData, m_canvasList->at(m_currentPage), m_model, m_currentPage);
        m_undoStack->push(deleteCommand);
    }
    else if(m_applyValueOnAllLines == act)
    {
        applyValue(index, false);
    }
    else if(m_applyValueOnSelection == act)
    {
        applyValue(index, true);
    }
    else if(m_defineCode == act)
    {
        defineItemCode(index);
    }
    else if(m_resetCode == act)
    {
        if(!index.isValid())
            return;

        Field* field= m_model->getFieldFromIndex(index);

        if(nullptr != field)
        {
            field->setGeneratedCode(QStringLiteral(""));
        }
    }
}

FieldModel* FieldView::getModel() const
{
    return m_model;
}

void FieldView::setFieldModel(FieldModel* model)
{
    m_model= model;
    setModel(m_model);
}

QList<Canvas*>* FieldView::getCanvasList() const
{
    return m_canvasList;
}

void FieldView::setCanvasList(QList<Canvas*>* canvasList)
{
    m_canvasList= canvasList;
}

int FieldView::getCurrentPage() const
{
    return m_currentPage;
}

void FieldView::setCurrentPage(int currentPage)
{
    m_currentPage= currentPage;
}
void FieldView::applyValue(QModelIndex& index, bool selection)
{
    if(!index.isValid())
        return;

    QList<CharacterSheetItem*> listField;
    QUndoCommand* cmd= nullptr;
    QVariant var= index.data(Qt::DisplayRole);
    QVariant editvar= index.data(Qt::EditRole);
    if(editvar != var)
    {
        var= editvar;
    }
    int col= index.column();

    if(selection)
    {
        QModelIndexList list= selectionModel()->selectedIndexes();

        for(QModelIndex& index : list)
        {
            if(index.column() == col)
            {
                auto field= static_cast<Field*>(index.internalPointer());
                listField.append(field);
            }
        }
        cmd= new SetFieldPropertyCommand(listField, var, col);
    }
    else
    {
        cmd= new SetFieldPropertyCommand(m_model->children(), var, col);
        m_model->setValueForAll(index);
    }
    m_undoStack->push(cmd);
}

void FieldView::defineItemCode(QModelIndex& index)
{
    if(!index.isValid())
        return;

    Field* field= m_model->getFieldFromIndex(index);

    if(nullptr != field)
    {
        CodeEditorDialog dialog;

        field->storeQMLCode();
        auto code= field->getGeneratedCode();
        dialog.setPlainText(code);

        if(dialog.exec())
        {
            field->setGeneratedCode(dialog.toPlainText());
        }
    }
}

QUndoStack* FieldView::getUndoStack() const
{
    return m_undoStack;
}

void FieldView::setUndoStack(QUndoStack* undoStack)
{
    m_undoStack= undoStack;
}

void FieldView::editColor(QModelIndex index)
{
    if(!index.isValid())
    {
        return;
    }
    if(index.column() == CharacterSheetItem::BGCOLOR || CharacterSheetItem::TEXTCOLOR == index.column())
    {

        CharacterSheetItem* itm= static_cast<CharacterSheetItem*>(index.internalPointer());

        if(nullptr != itm)
        {
            QColor col= itm->getValueFrom(static_cast<CharacterSheetItem::ColumnId>(index.column()), Qt::EditRole)
                            .value<QColor>(); // CharacterSheetItem::TEXTCOLOR
            col= QColorDialog::getColor(col, this, tr("Get Color"), QColorDialog::ShowAlphaChannel);

            itm->setValueFrom(static_cast<CharacterSheetItem::ColumnId>(index.column()), col);
        }
    }
}
