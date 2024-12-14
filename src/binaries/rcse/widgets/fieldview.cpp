#include "fieldview.h"

#include <QColorDialog>
#include <QFontDialog>
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

#include "controllers/maincontroller.h"

#include "dialog/codeeditordialog.h"

FieldView::FieldView(QWidget* parent) : QTreeView(parent), m_mapper(new QSignalMapper(this))
{
    // Item action
    m_lock= new QAction(tr("Lock up Item"), this);
    m_delItem= new QAction(tr("Delete Item"), this);
    m_applyValueOnSelection= new QAction(tr("Apply on Selection"), this);
    m_applyValueOnAllLines= new QAction(tr("Apply on all lines"), this);
    m_defineCode= new QAction(tr("Define Field Code"), this);
    m_resetCode= new QAction(tr("Reset Field Code"), this);

    connect(m_lock, &QAction::triggered, this, &FieldView::lockItems);

    m_showGeometryGroup= new QAction(tr("Position columns"), this);
    connect(m_showGeometryGroup, &QAction::triggered, this,
            [=]()
            {
                hideAllColumns(true);
                showColumn(TreeSheetItem::ID);
                showColumn(TreeSheetItem::LABEL);
                showColumn(TreeSheetItem::X);
                showColumn(TreeSheetItem::Y);
                showColumn(TreeSheetItem::WIDTH);
                showColumn(TreeSheetItem::HEIGHT);
                showColumn(TreeSheetItem::PAGE);
            });
    m_showEsteticGroup= new QAction(tr("Aesthetic columns"), this);
    connect(m_showEsteticGroup, &QAction::triggered, this,
            [=]()
            {
                hideAllColumns(true);
                showColumn(TreeSheetItem::ID);
                showColumn(TreeSheetItem::LABEL);
                showColumn(TreeSheetItem::BGCOLOR);
                showColumn(TreeSheetItem::BORDER);
                showColumn(TreeSheetItem::FitFont);
                showColumn(TreeSheetItem::FONT);
                showColumn(TreeSheetItem::TEXT_ALIGN);
                showColumn(TreeSheetItem::TEXTCOLOR);
            });
    m_showValueGroup= new QAction(tr("Value columns"), this);
    connect(m_showValueGroup, &QAction::triggered, this,
            [=]()
            {
                hideAllColumns(true);
                showColumn(TreeSheetItem::ID);
                showColumn(TreeSheetItem::LABEL);
                showColumn(TreeSheetItem::VALUE);
                showColumn(TreeSheetItem::VALUES);
                showColumn(TreeSheetItem::TYPE);
            });
    m_showIdGroup= new QAction(tr("Id columns"), this);
    connect(m_showIdGroup, &QAction::triggered, this,
            [=]()
            {
                hideAllColumns(true);
                showColumn(TreeSheetItem::ID);
                showColumn(TreeSheetItem::LABEL);
            });

    m_showAllGroup= new QAction(tr("All columns"), this);
    connect(m_showAllGroup, &QAction::triggered, this, [=]() { hideAllColumns(false); });

#ifndef Q_OS_MACX
    setAlternatingRowColors(true);
#endif

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editColor(QModelIndex)));

    AlignmentDelegate* delegate= new AlignmentDelegate(this);
    setItemDelegateForColumn(static_cast<int>(TreeSheetItem::TEXT_ALIGN), delegate);

    TypeDelegate* typeDelegate= new TypeDelegate(this);
    setItemDelegateForColumn(static_cast<int>(TreeSheetItem::TYPE), typeDelegate);

    /*FontDelegate* fontDelegate= new FontDelegate(this);
    setItemDelegateForColumn(static_cast<int>(TreeSheetItem::FONT), fontDelegate);*/

    PageDelegate* pageDelegate= new PageDelegate(this);
    setItemDelegateForColumn(static_cast<int>(TreeSheetItem::PAGE), pageDelegate);

    setItemDelegateForColumn(TreeSheetItem::BORDER, new BorderListEditor);

    connect(m_mapper, &QSignalMapper::mappedInt, this,
            [this](int i) { this->setColumnHidden(i, !this->isColumnHidden(i)); });
}

void FieldView::setController(rcse::MainController* ctrl)
{
    m_ctrl= ctrl;

    setFieldModel(m_ctrl->generatorCtrl()->fieldModel());
    connect(m_ctrl->editCtrl(), &EditorController::pageCountChanged, this,
            [this]() { setCurrentPage(static_cast<int>(m_ctrl->editCtrl()->pageCount())); });

    connect(this, &FieldView::removeField, this,
            [this](FieldController* field, int currentPage)
            {
                m_ctrl->processCommand(new DeleteFieldCommand(field, m_ctrl->editCtrl()->currentCanvas(),
                                                              m_ctrl->generatorCtrl()->fieldModel(), currentPage));
            });
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
        menu.addAction(m_lock);
        menu.addAction(m_delItem);
        menu.addSeparator();
        menu.addAction(m_defineCode);
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

    if(act == nullptr)
        return;

    if(act == m_delItem)
    {
        auto itemData= static_cast<FieldController*>(index.internalPointer());
        /*DeleteFieldCommand* deleteCommand
            = new DeleteFieldCommand(itemData, m_model, m_currentPage);
        m_undoStack->push(deleteCommand);*/
        emit removeField(itemData, m_currentPage);
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

        FieldController* field= m_model->getFieldFromIndex(index);

        if(nullptr != field)
        {
            field->setGeneratedCode(QStringLiteral(""));
        }
    }
}
void FieldView::lockItems()
{
    QModelIndexList list= selectionModel()->selectedIndexes();

    if(list.isEmpty())
        return;

    std::for_each(list.begin(), list.end(),
                  [](QModelIndex& index)
                  {
                      if(!index.isValid())
                          return;

                      auto field= static_cast<FieldController*>(index.internalPointer());
                      auto value= field->readOnly();
                      field->setReadOnly(!value);
                  });
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

    QList<TreeSheetItem*> listField;
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
                auto field= static_cast<FieldController*>(index.internalPointer());
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
    m_ctrl->processCommand(cmd);
}

void FieldView::defineItemCode(QModelIndex& index)
{
    if(!index.isValid())
        return;

    FieldController* field= m_model->getFieldFromIndex(index);

    if(nullptr != field)
    {
        CodeEditorDialog dialog;

        // field->storeQMLCode();
        auto code= field->generatedCode();
        dialog.setPlainText(code);

        if(dialog.exec())
        {
            field->setGeneratedCode(dialog.toPlainText());
        }
    }
}

void FieldView::editColor(QModelIndex index)
{
    if(!index.isValid())
    {
        return;
    }
    auto col= index.column();
    if(col == TreeSheetItem::BGCOLOR || TreeSheetItem::TEXTCOLOR == col)
    {

        auto itm= dynamic_cast<CSItem*>(static_cast<TreeSheetItem*>(index.internalPointer()));

        if(nullptr == itm)
            return;

        QColor col= itm->valueFrom(static_cast<CSItem::ColumnId>(index.column()), Qt::EditRole)
                        .value<QColor>(); // TreeSheetItem::TEXTCOLOR
        col= QColorDialog::getColor(col, this, tr("Get Color"), QColorDialog::ShowAlphaChannel);

        itm->setValueFrom(static_cast<TreeSheetItem::ColumnId>(index.column()), col);
    }
    else if(TreeSheetItem::FONT == col)
    {
        QFontDialog cd(this);
        QString fontStr= index.data(Qt::EditRole).toString();
        QFont font;
        font.fromString(fontStr);

        cd.setCurrentFont(font);
        cd.exec();
        model()->setData(index, cd.currentFont(), Qt::EditRole);
    }
}
