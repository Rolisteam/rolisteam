#include "sheetproperties.h"
#include "ui_sheetproperties.h"

#include <QFileDialog>
#include <QFontDatabase>

#include "controllers/qmlgeneratorcontroller.h"

SheetProperties::SheetProperties(QmlGeneratorController* ctrl, QWidget* parent)
    : QDialog(parent), ui(new Ui::SheetProperties), m_ctrl(ctrl)
{
    ui->setupUi(this);
    ui->listView->setModel(&m_model);

    connect(ui->m_addFontBtn, &QToolButton::clicked, this, [=] {
        QStringList files
            = QFileDialog::getOpenFileNames(this, tr("Add Font file"), QDir::homePath(), "font (*.ttf *.otf)");
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

    connect(ui->m_removeFontBtn, &QToolButton::clicked, this, [=] {
        auto index= ui->listView->currentIndex();
        if(index.isValid())
        {
            m_fontUri.removeAt(index.row());
            m_model.setStringList(m_fontUri);
        }
    });

    init();
}

SheetProperties::~SheetProperties()
{
    delete ui;
}

void SheetProperties::init()
{
    ui->m_flickable->setChecked(m_ctrl->flickable());
    ui->m_additionnalHeadCode->document()->setPlainText(m_ctrl->headCode());
    ui->m_additionnalBottomCode->document()->setPlainText(m_ctrl->bottomCode());
    ui->m_additionnalImport->setPlainText(m_ctrl->importCode());
    ui->m_fixedScale->setValue(m_ctrl->fixedScale());

    m_fontUri= m_ctrl->fonts();
    m_model.setStringList(m_fontUri);
}

void SheetProperties::accept()
{
    m_ctrl->setFlickable(ui->m_flickable->isChecked());
    m_ctrl->setHeadCode(ui->m_additionnalHeadCode->document()->toPlainText());
    m_ctrl->setBottomCode(ui->m_additionnalBottomCode->document()->toPlainText());
    m_ctrl->setImportCode(ui->m_additionnalImport->document()->toPlainText());
    m_ctrl->setFixedScale(ui->m_fixedScale->value());
    m_ctrl->setFonts(m_fontUri);

    QDialog::accept();
}
