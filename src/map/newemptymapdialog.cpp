#include <QDebug>


#include "map/newemptymapdialog.h"
#include "ui_newemptymapdialog.h"

#include "preferences/preferencesmanager.h"

QStringList  NewEmptyMapDialog::m_permissionData = QStringList() ;

NewEmptyMapDialog::NewEmptyMapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewEmptyMapDialog)
{
    ui->setupUi(this);
    m_colorButton = new ColorButton(Qt::white,this);
    ui->m_backgroundLayout->addWidget(m_colorButton);
    m_labelModel << tr("Small (%1 x %2)") << tr("Middle (%1 x %2)")<< tr("Big (%1 x %2)")<< tr("Huge (%1 x %2)");
    m_landscapeModel << QSize(600,450) << QSize(800,600)<< QSize(1000,750)<< QSize(1200,900);
    m_portraitModel << QSize(450,600) << QSize(600,800)<< QSize(750,1000)<< QSize(900,1200);
    m_squareModel << QSize(500,500) << QSize(700,700)<< QSize(900,900)<< QSize(1100,1100);


    connect(ui->m_landscapeCheckbox,SIGNAL(clicked()),this,SLOT(updateSize()));
    connect(ui->m_portraitCheckbox,SIGNAL(clicked()),this,SLOT(updateSize()));
    connect(ui->m_sqareCheckbox,SIGNAL(clicked()),this,SLOT(updateSize()));

    m_permissionData   << tr("No Right") << tr("His character") << tr("All Permissions");
    ui->m_permissionSelector->addItems(m_permissionData);

  //  ui->m_permissionSelector->setCurrentIndex(PreferencesManager::getInstance()->value("defaultPermissionMap",0).toInt());
    updateSize();
}

NewEmptyMapDialog::~NewEmptyMapDialog()
{
    delete ui;
}

void NewEmptyMapDialog::resetData()
{
    ui->m_titleEdit->setText("");
    ui->m_permissionSelector->setCurrentIndex(PreferencesManager::getInstance()->value("defaultPermissionMap",0).toInt());
    m_colorButton->setColor(Qt::white);
    ui->m_landscapeCheckbox->setChecked(true);
    ui->m_middleRadio->setChecked(true);
    updateSize();
}
QStringList NewEmptyMapDialog::getPermissionData()
{
    return m_permissionData;
}

void NewEmptyMapDialog::updateSize()
{

    QList<QSize> tmpModel;
    if(ui->m_landscapeCheckbox->isChecked())
    {
            tmpModel = m_landscapeModel;
    }
    else if(ui->m_portraitCheckbox->isChecked())
    {
        tmpModel = m_portraitModel;
    }
    else
    {
        tmpModel = m_squareModel;
    }
    //small
    ui->m_smallRadio->setText(m_labelModel.at(0).arg(tmpModel.at(0).width()).arg(tmpModel.at(0).height()));
    //middle
    ui->m_middleRadio->setText(m_labelModel.at(1).arg(tmpModel.at(1).width()).arg(tmpModel.at(1).height()));
    //big
    ui->m_bigRadio->setText(m_labelModel.at(2).arg(tmpModel.at(2).width()).arg(tmpModel.at(2).height()));
    //huge
    ui->m_hugeRadio->setText(m_labelModel.at(3).arg(tmpModel.at(3).width()).arg(tmpModel.at(3).height()));
}
Map::PermissionMode NewEmptyMapDialog::getPermission() const
{
    Map::PermissionMode result;
    switch (ui->m_permissionSelector->currentIndex())
    {
    case 0:
        result = Map::GM_ONLY;
        break;
    case 1:
        result = Map::PC_MOVE;
        break;
    case 2:
        result = Map::PC_ALL;
        break;
    default:
        result = Map::GM_ONLY;
        break;
    }
  return result;
}
QString NewEmptyMapDialog::getTitle( ) const
{
 return ui->m_titleEdit->text();
}
QColor NewEmptyMapDialog::getColor( ) const
{
    return m_colorButton->color();
}
QSize NewEmptyMapDialog::getSize() const
{
    QList<QSize> tmpModel;
    if(ui->m_landscapeCheckbox->isChecked())
    {
            tmpModel = m_landscapeModel;
    }
    else if(ui->m_portraitCheckbox->isChecked())
    {
        tmpModel = m_portraitModel;
    }
    else
    {
        tmpModel = m_squareModel;
    }


    if(ui->m_smallRadio->isChecked())//small
    {
         return tmpModel.at(0);
    }
    else if(ui->m_middleRadio->isChecked())//middle
    {
        return tmpModel.at(1);
    }
     else if(ui->m_bigRadio->isChecked())//big
    {
         return tmpModel.at(2);
    }
    else if(ui->m_customRadio->isChecked())
    {
        QSize result;

        int sizeW = ui->m_widthEdit->text().toInt();
        int sizeH= ui->m_heightEdit->text().toInt();
        result.setHeight(sizeH);
        result.setWidth(sizeW);
        return result;

    }
    else//huge
    {
         return tmpModel.at(3);
    }


}
