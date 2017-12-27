#include "fieldview.h"

#include <QMenu>
#include <QColorDialog>

//commands
#include "undo/deletefieldcommand.h"
#include "undo/setfieldproperties.h"


// Delegates
#include "delegate/alignmentdelegate.h"
#include "delegate/typedelegate.h"
#include "delegate/fontdelegate.h"
#include "delegate/pagedelegate.h"
#include "borderlisteditor.h"


#include "codeeditordialog.h"


FieldView::FieldView(QWidget* parent)
    : QTreeView(parent)
{
    //Item action
    m_delItem = new QAction(tr("Delete Item"),this);
    m_applyValueOnSelection = new QAction(tr("Apply on Selection"),this);
    m_applyValueOnAllLines = new QAction(tr("Apply on all lines"),this);
    m_defineCode = new QAction(tr("Define Field Code"),this);
    m_resetCode = new QAction(tr("Reset Field Code"),this);

    setAlternatingRowColors(true);
#ifdef Q_OS_MACX
    setAlternatingRowColors(false);
#endif

    connect(this,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editColor(QModelIndex)));

    AlignmentDelegate* delegate = new AlignmentDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::TEXT_ALIGN),delegate);

    TypeDelegate* typeDelegate = new TypeDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::TYPE),typeDelegate);

    FontDelegate* fontDelegate = new FontDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::FONT),fontDelegate);

    PageDelegate* pageDelegate = new PageDelegate(this);
    setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::PAGE),pageDelegate);

    setItemDelegateForColumn(CharacterSheetItem::BORDER,new BorderListEditor);


}

void FieldView::contextMenuEvent(QContextMenuEvent* event)
{

    QMenu menu(this);

    QModelIndex index = currentIndex();
    if(index.isValid())
    {

        menu.addAction(m_applyValueOnSelection);
        menu.addAction(m_applyValueOnAllLines);
        menu.addSeparator();
        menu.addAction(m_defineCode);
        menu.addSeparator();
        menu.addAction(m_delItem);
    }

    QAction* act = menu.exec(event->globalPos());

    if(act == m_delItem)
    {
        auto itemData = static_cast<Field*>(index.internalPointer());
        DeleteFieldCommand* deleteCommand = new DeleteFieldCommand(itemData,m_canvasList->at(*m_currentPage),m_model,*m_currentPage);
        m_undoStack->push(deleteCommand);
    }
    else if( m_applyValueOnAllLines == act)
    {
        applyValue(index,false);
    }
    else if(m_applyValueOnSelection == act)
    {
        applyValue(index,true);
    }
    else if(m_defineCode == act)
    {
        defineItemCode(index);
    }
    else if(m_resetCode == act)
    {
        if(!index.isValid())
            return;

        Field* field = m_model->getFieldFromIndex(index);

        if(nullptr != field)
        {
            field->setGeneratedCode(QStringLiteral(""));
        }

    }
}

FieldModel *FieldView::getModel() const
{
    return m_model;
}

void FieldView::setFieldModel(FieldModel *model)
{
    m_model = model;
    setModel(m_model);
}

QList<Canvas *> *FieldView::getCanvasList() const
{
    return m_canvasList;
}

void FieldView::setCanvasList(QList<Canvas *> *canvasList)
{
    m_canvasList = canvasList;
}

int *FieldView::getCurrentPage() const
{
    return m_currentPage;
}

void FieldView::setCurrentPage(int *currentPage)
{
    m_currentPage = currentPage;
}
void FieldView::applyValue(QModelIndex& index, bool selection)
{
    if(!index.isValid())
        return;

    QList<CharacterSheetItem*> listField;
    QUndoCommand* cmd = nullptr;
    QVariant var = index.data(Qt::DisplayRole);
    QVariant editvar = index.data(Qt::EditRole);
    if(editvar != var)
    {
        var = editvar;
    }
    int col = index.column();

    if(selection)
    {
        QModelIndexList list = selectionModel()->selectedIndexes();

        for(QModelIndex& index : list)
        {
            if(index.column() == col)
            {
                auto field = static_cast<Field*>(index.internalPointer());
                listField.append(field);
            }
        }
        cmd = new SetFieldPropertyCommand(m_model,listField,var,col);
    }
    else
    {
        cmd = new SetFieldPropertyCommand(m_model,m_model->children(),var,col);
        m_model->setValueForAll(index);
    }
    m_undoStack->push(cmd);
}

void FieldView::defineItemCode(QModelIndex& index)
{
    if(!index.isValid())
        return;

    Field* field = m_model->getFieldFromIndex(index);

    if(nullptr != field)
    {
        CodeEditorDialog dialog;

        field->storeQMLCode();
        auto code = field->getGeneratedCode();
        dialog.setPlainText(code);

        if(dialog.exec())
        {
           field->setGeneratedCode(dialog.toPlainText());
        }
    }

}

QUndoStack *FieldView::getUndoStack() const
{
    return m_undoStack;
}

void FieldView::setUndoStack(QUndoStack *undoStack)
{
    m_undoStack = undoStack;
}

void FieldView::editColor(QModelIndex index)
{
    if(!index.isValid())
    {
        return;
    }
    if(index.column()==CharacterSheetItem::BGCOLOR || CharacterSheetItem::TEXTCOLOR == index.column())
    {

        CharacterSheetItem* itm = static_cast<CharacterSheetItem*>(index.internalPointer());

        if(nullptr!=itm)
        {
            QColor col = itm->getValueFrom(static_cast<CharacterSheetItem::ColumnId>(index.column()),Qt::EditRole).value<QColor>();//CharacterSheetItem::TEXTCOLOR
            col = QColorDialog::getColor(col,this,tr("Get Color"),QColorDialog::ShowAlphaChannel);

            itm->setValueFrom(static_cast<CharacterSheetItem::ColumnId>(index.column()),col);
        }
    }
}
