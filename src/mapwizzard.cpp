#include <QFileInfo>
#include <QFileDialog>

#include "mapwizzard.h"
#include "ui_mapwizzard.h"



MapWizzard::MapWizzard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapWizzard)
{
    ui->setupUi(this);

    m_preferences = PreferencesManager::getInstance();
    connect(ui->m_browserPush,SIGNAL(pressed()),this,SLOT(selectPath()));
    QStringList data;
    data << tr("No Right") << tr("His character") << tr("All Permissions");
    ui->m_permissionSelector->addItems(data);
}

MapWizzard::~MapWizzard()
{
    delete ui;
}
void MapWizzard::selectPath()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Select Map"), m_preferences->value("MapDirectory",QDir::homePath()).toString(),
                                      tr("Map (*.pla *.jpg *.jpeg *.png *.bmp)"));
    ui->m_filepathDisplay->setText(filepath);


    if(ui->m_titleEdit->text().isEmpty())
    {
        QFileInfo info(filepath);
        ui->m_titleEdit->setText(info.baseName());
    }
}
Carte::PermissionMode  MapWizzard::getPermissionMode() const
{
    Carte::PermissionMode result;
    switch (ui->m_permissionSelector->currentIndex()) {
    case 0:
        result = Carte::GM_ONLY;
        break;
    case 1:
        result = Carte::PC_MOVE;
        break;
    case 2:
        result = Carte::PC_ALL;
        break;
    default:
        result = Carte::GM_ONLY;
        break;
    }
  return result;
}

QString  MapWizzard::getFilepath() const
{
    return ui->m_filepathDisplay->text();
}

bool  MapWizzard::getHidden() const
{
    return ui->m_hiddenCheckbox->isChecked();
}

QString  MapWizzard::getTitle() const
{
    return ui->m_titleEdit->text();
}

void MapWizzard::resetData()
{
    ui->m_titleEdit->setText("");
    ui->m_hiddenCheckbox->setChecked(false);
    ui->m_permissionSelector->setCurrentIndex(0);
    ui->m_filepathDisplay->setText("");
}
