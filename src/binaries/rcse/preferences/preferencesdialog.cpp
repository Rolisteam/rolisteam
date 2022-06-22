#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QFileDialog>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    connect(ui->m_selectDirectory,SIGNAL(clicked(bool)),this,SLOT(selectDir()));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

QString PreferencesDialog::generationPath() const
{
    return ui->m_dirPath->text();
}

void PreferencesDialog::setGenerationPath(const QString &generationPath)
{
    ui->checkBox->setCheckState(Qt::Checked);
    ui->m_dirPath->setText(generationPath);
}
bool PreferencesDialog::hasCustomPath()
{
    return (ui->checkBox->checkState() == Qt::Checked);
}
void PreferencesDialog::selectDir()
{
    QString path = QFileDialog::getExistingDirectory(this,tr("Directory to save QML file"),tr("Place to save Generated files"));
    if(!path.isEmpty())
    {
        ui->m_dirPath->setText(path);
    }
}
