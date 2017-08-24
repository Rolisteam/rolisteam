#include "columndefinitiondialog.h"
#include "ui_columndefinitiondialog.h"

#include "borderlisteditor.h"
#include "tablecanvasfield.h"
#include "delegate/alignmentdelegate.h"
#include "delegate/typedelegate.h"
#include "delegate/fontdelegate.h"


ColumnDefinitionDialog::ColumnDefinitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnDefinitionDialog)
{
    ui->setupUi(this);
    m_model = new FieldModel(this);
    ui->m_column2Field->setModel(m_model);

    AlignmentDelegate* delegate = new AlignmentDelegate();
    ui->m_column2Field->setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::TEXT_ALIGN),delegate);

    TypeDelegate* typeDelegate = new TypeDelegate();
    ui->m_column2Field->setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::TYPE),typeDelegate);

    FontDelegate* fontDelegate = new FontDelegate();
    ui->m_column2Field->setItemDelegateForColumn(static_cast<int>(CharacterSheetItem::FONT),fontDelegate);

    ui->m_column2Field->setItemDelegateForColumn(CharacterSheetItem::BORDER,new BorderListEditor);

}

ColumnDefinitionDialog::~ColumnDefinitionDialog()
{
    delete ui;
}

void ColumnDefinitionDialog::setData(QList<HandleItem *> list,qreal widthTotal)
{
    qreal currentX = 0;
    m_model->clearModel();
    for(auto handle : list)
    {
        auto field = new Field();
        field->setCurrentType(CharacterSheetItem::TEXTINPUT);
        m_model->appendField(field);
        field->setX(currentX);
        field->setWidth(handle->pos().x()-currentX);
        currentX  = handle->pos().x();
    }
    auto field = new Field();
    field->setCurrentType(CharacterSheetItem::TEXTINPUT);
    m_model->appendField(field);
    field->setX(currentX);
    field->setWidth(widthTotal-currentX);
}

FieldModel *ColumnDefinitionDialog::model() const
{
    return m_model;
}

void ColumnDefinitionDialog::setModel(FieldModel *model)
{
    m_model = model;
}
