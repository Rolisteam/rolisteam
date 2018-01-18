#include "columndefinitiondialog.h"
#include <QUndoStack>


#include "ui_columndefinitiondialog.h"
#include "borderlisteditor.h"
#include "tablecanvasfield.h"
#include "delegate/alignmentdelegate.h"
#include "delegate/typedelegate.h"
#include "delegate/fontdelegate.h"
#include "delegate/pagedelegate.h"


ColumnDefinitionDialog::ColumnDefinitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnDefinitionDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Table Properties"));
    m_model = new FieldModel(this);
    ui->m_column2Field->setFieldModel(m_model);
    ui->m_column2Field->setCurrentPage(nullptr);
    ui->m_column2Field->setCanvasList(nullptr);
    ui->m_column2Field->setUndoStack(new QUndoStack(this));

    connect(ui->m_columnCountEdit,SIGNAL(valueChanged(int)),this,SIGNAL(columnCountChanged(int)));
    connect(ui->m_lineCountEdit,SIGNAL(valueChanged(int)),this,SIGNAL(lineCountChanged(int)));
    connect(ui->m_controlPositionCb,SIGNAL(currentIndexChanged(int)),this,SIGNAL(positionChanged(int)));
}

ColumnDefinitionDialog::~ColumnDefinitionDialog()
{
    delete ui;
}

void ColumnDefinitionDialog::setData(QList<HandleItem *> list,qreal widthTotal, int line, qreal height)
{
    qreal currentX = 0;
    qreal currentY = height/line;
    m_model->clearModel();
    int i = 0;
    for(auto handle : list)
    {
        auto field = new Field();
        field->setCurrentType(CharacterSheetItem::TEXTINPUT);
        m_model->appendField(field);
        field->setX(currentX);
        field->setY(i*currentY);
        field->setWidth(handle->pos().x()-currentX);
        field->setHeight(currentY);
        currentX  = handle->pos().x();
        ++i;
    }
    auto field = new Field();
    field->setCurrentType(CharacterSheetItem::TEXTINPUT);
    m_model->appendField(field);
    field->setX(currentX);
    field->setY(i*currentY);
    field->setWidth(widthTotal-currentX);
    field->setHeight(currentY);


    ui->m_columnCountEdit->setValue(list.size()+1);
    ui->m_lineCountEdit->setValue(line);
}

FieldModel* ColumnDefinitionDialog::model() const
{
    return m_model;
}

void ColumnDefinitionDialog::setModel(FieldModel *model)
{
    m_model = model;
}
void ColumnDefinitionDialog::load(QJsonObject &json, QList<QGraphicsScene *> scene)
{
    m_model->load(json, scene);
}
void ColumnDefinitionDialog::save(QJsonObject &json)
{
    m_model->save(json);
}
/*void ColumnDefinitionDialog::positionChanged(int i)
{

}
void ColumnDefinitionDialog::lineCountChanged(int i)
{

}
void ColumnDefinitionDialog::columnCountChanged(int i)
{

}*/
