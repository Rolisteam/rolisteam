#include "sheetproperties.h"
#include "ui_sheetproperties.h"

#include <QFileDialog>

SheetProperties::SheetProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SheetProperties)
{
    ui->setupUi(this);
    ui->listView->setModel(&m_model);

    connect(ui->m_addFontBtn,&QToolButton::clicked,this,[=]{
        QStringList files = QFileDialog::getOpenFileNames(this,tr("Add Font file"),QDir::homePath(),"font (*.ttf *.otf)");
        if(!files.isEmpty())
        {
            m_fontUri.append(files);
            m_model.setStringList(m_fontUri);
        }
    });

    connect(ui->m_removeFontBtn,&QToolButton::clicked,this,[=]{
        auto index = ui->listView->currentIndex();
        if(index.isValid())
        {
            m_fontUri.removeAt(index.row());
            m_model.setStringList(m_fontUri);
        }
    });
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

QStringList SheetProperties::getFontUri() const
{
    return m_fontUri;
}

void SheetProperties::setFontUri(const QStringList &fontUri)
{
    m_fontUri = fontUri;
    m_model.setStringList(m_fontUri);
}

