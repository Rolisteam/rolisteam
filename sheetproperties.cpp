#include "sheetproperties.h"
#include "ui_sheetproperties.h"

SheetProperties::SheetProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SheetProperties)
{
    ui->setupUi(this);
}

SheetProperties::~SheetProperties()
{
    delete ui;
}

bool SheetProperties::isNoAdaptation() const
{
    return ui->m_flickable->isChecked();
}

void SheetProperties::setNoAdaptation(bool noAdaptation)
{
    ui->m_flickable->setChecked(noAdaptation);
}

QString SheetProperties::getAdditionalCode() const
{
    return  ui->m_additionnalCode->document()->toPlainText();
}

void SheetProperties::setAdditionalCode(const QString &additionalCode)
{
    ui->m_additionnalCode->document()->setPlainText(additionalCode);
}

QString SheetProperties::getAdditionalImport() const
{
    return ui->m_additionnalImport->document()->toPlainText();
}

void SheetProperties::setAdditionalImport(const QString &additionalImport)
{
    ui->m_additionnalImport->setPlainText(additionalImport);
}

qreal SheetProperties::getFixedScale() const
{
    return ui->m_fixedScale->value();
}

void SheetProperties::setFixedScale(const qreal &fixedScale)
{
    ui->m_fixedScale->setValue(fixedScale);
}

bool SheetProperties::getAdditionCodeAtTheBeginning() const
{
    return  (ui->m_placeToAdditionnal->currentIndex() == 0);
}

void SheetProperties::setAdditionCodeAtTheBeginning(bool additionCodeAtTheBeginning)
{
    if(additionCodeAtTheBeginning)
    {
         ui->m_placeToAdditionnal->setCurrentIndex(0);
    }
    else
    {
        ui->m_placeToAdditionnal->setCurrentIndex(1);
    }
}

void SheetProperties::reset()
{
    ui->m_additionnalCode->clear();
    ui->m_flickable->setChecked(false);
    ui->m_fixedScale->setValue(1.0);
    ui->m_placeToAdditionnal->setCurrentIndex(0);
}

