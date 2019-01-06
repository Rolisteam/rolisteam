#include "sheetproperties.h"
#include "ui_sheetproperties.h"

#include <QFileDialog>
#include <QFontDatabase>

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
            for(auto uri : m_fontUri)
            {
                QFontDatabase::addApplicationFont(uri);
            }
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

QString SheetProperties::getAdditionalHeadCode() const
{
    return  ui->m_additionnalHeadCode->document()->toPlainText();
}

void SheetProperties::setAdditionalHeadCode(const QString &additionalHeadCode)
{
    ui->m_additionnalHeadCode->document()->setPlainText(additionalHeadCode);
}

QString SheetProperties::getAdditionalBottomCode() const
{
    return  ui->m_additionnalBottomCode->document()->toPlainText();
}

void SheetProperties::setAdditionalBottomCode(const QString &additionalBottomCode)
{
    ui->m_additionnalBottomCode->document()->setPlainText(additionalBottomCode);
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

void SheetProperties::reset()
{
    ui->m_additionnalHeadCode->clear();
    ui->m_additionnalBottomCode->clear();
    ui->m_flickable->setChecked(false);
    ui->m_fixedScale->setValue(1.0);
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

