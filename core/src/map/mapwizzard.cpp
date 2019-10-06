#include <QFileDialog>
#include <QFileInfo>

#include "map/mapwizzard.h"
#include "ui_mapwizzard.h"

MapWizzard::MapWizzard(bool vmap, QWidget* parent) : QDialog(parent), ui(new Ui::MapWizzard)
{
    m_vmap= vmap;
    ui->setupUi(this);

    m_preferences= PreferencesManager::getInstance();
    connect(ui->m_browserPush, SIGNAL(clicked()), this, SLOT(selectPath()));
    QStringList data;
    data << tr("No Right") << tr("His character") << tr("All Permissions");
    ui->m_permissionSelector->addItems(data);

    if(m_vmap)
    {
        ui->label_3->setVisible(false);
        ui->label_2->setVisible(false);
        ui->m_permissionSelector->setVisible(false);
        ui->m_titleEdit->setVisible(false);
        ui->m_hiddenCheckbox->setVisible(false);
    }
}

MapWizzard::~MapWizzard()
{
    delete ui;
}
void MapWizzard::selectPath()
{
    QString filepath;
    if(m_vmap)
    {
        filepath= QFileDialog::getOpenFileName(this, tr("Select Map"),
            m_preferences->value("MapDirectory", QDir::homePath()).toString(), tr("Map (*.vmap)"));
    }
    else
    {
        filepath= QFileDialog::getOpenFileName(this, tr("Select Map"),
            m_preferences->value("MapDirectory", QDir::homePath()).toString(),
            tr("Map (*.pla *.jpg *.jpeg *.png *.bmp)"));
    }
    ui->m_filepathDisplay->setText(filepath);

    if(ui->m_titleEdit->text().isEmpty())
    {
        QFileInfo info(filepath);
        ui->m_titleEdit->setText(info.baseName());
    }
}
Map::PermissionMode MapWizzard::getPermissionMode() const
{
    Map::PermissionMode result;
    switch(ui->m_permissionSelector->currentIndex())
    {
    case 0:
        result= Map::GM_ONLY;
        break;
    case 1:
        result= Map::PC_MOVE;
        break;
    case 2:
        result= Map::PC_ALL;
        break;
    default:
        result= Map::GM_ONLY;
        break;
    }
    return result;
}

QString MapWizzard::getFilepath() const
{
    return ui->m_filepathDisplay->text();
}

bool MapWizzard::getHidden() const
{
    return ui->m_hiddenCheckbox->isChecked();
}

QString MapWizzard::getTitle() const
{
    return ui->m_titleEdit->text();
}

void MapWizzard::resetData()
{
    ui->m_titleEdit->setText("");
    ui->m_hiddenCheckbox->setChecked(false);
    ui->m_permissionSelector->setCurrentIndex(m_preferences->value("defaultPermissionMap", 0).toInt());
    ui->m_filepathDisplay->setText("");
}
