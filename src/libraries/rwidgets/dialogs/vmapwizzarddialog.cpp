/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QButtonGroup>
#include <QColorDialog>

#include "ui_vmapwizzarddialog.h"
#include "vmapwizzarddialog.h"

#include "preferences/preferencesmanager.h"

/// @todo make those value to be store in preferences.
#define DEFAULT_VMAP_WIDTH 800
#define DEFAULT_VMAP_HEIGHT 600

MapWizzardDialog::MapWizzardDialog(PreferencesManager* pref, QWidget* parent)
    : QDialog(parent), ui(new Ui::MapWizzardDialog), m_options(pref)
{
    ui->setupUi(this);
    m_model= new PatternModel();

    ui->m_gridPattern->setModel(m_model);

    m_permissionData << tr("No Right") << tr("His character") << tr("All Permissions");
    ui->m_permissionComboBox->addItems(m_permissionData);

    m_visibilityData << tr("Hidden") << tr("Fog of War") << tr("All visible");
    ui->m_visibilityComboBox->addItems(m_visibilityData);
    ui->m_gridColorBtn->setColor(QColor(0, 0, 0));

    if(m_options)
    {
        QColor bgColor= m_options->value(QString("MapWizzard/backgroundcolor"), QColor(255, 255, 255)).value<QColor>();
        ui->m_colorButton->setColor(bgColor);
        ui->m_permissionComboBox->setCurrentIndex(m_options->value("defaultPermissionMap", 0).toInt());
        ui->m_visibilityComboBox->setCurrentIndex(m_options->value("defaultMapVisibility", 0).toInt());
    }

    // selectedShapeChanged();
}

MapWizzardDialog::~MapWizzardDialog()
{
    delete ui;
}

QString MapWizzardDialog::name() const
{
    return ui->m_titleLineedit->text();
}

QColor MapWizzardDialog::backgroundColor() const
{
    return ui->m_colorButton->color();
}

int MapWizzardDialog::gridSize() const
{
    return ui->m_sizeGrid->value();
}

QColor MapWizzardDialog::gridColor() const
{
    return Qt::black;
}

Core::ScaleUnit MapWizzardDialog::unit() const
{
    return static_cast<Core::ScaleUnit>(ui->m_unitPattern->currentIndex());
}
qreal MapWizzardDialog::scale() const
{
    return ui->m_scaleOfGrid->value();
}

Core::GridPattern MapWizzardDialog::pattern() const
{
    Core::GridPattern grid;
    switch(ui->m_gridPattern->currentIndex())
    {
    case 0:
        grid= Core::GridPattern::NONE;
        break;
    case 1:
        grid= Core::GridPattern::SQUARE;
        break;
    case 3:
        grid= Core::GridPattern::OCTOGON;
        break;
    case 2:
        grid= Core::GridPattern::HEXAGON;
        break;
    default:
        grid= Core::GridPattern::NONE;
        break;
    }
    return grid;
}

Core::PermissionMode MapWizzardDialog::permission() const
{
    Core::PermissionMode result;
    switch(ui->m_permissionComboBox->currentIndex())
    {
    case 0:
        result= Core::GM_ONLY;
        break;
    case 1:
        result= Core::PC_MOVE;
        break;
    case 2:
        result= Core::PC_ALL;
        break;
    default:
        result= Core::GM_ONLY;
        break;
    }
    return result;
}

Core::VisibilityMode MapWizzardDialog::visibility() const
{
    Core::VisibilityMode resultVisibility= Core::HIDDEN;
    switch(ui->m_visibilityComboBox->currentIndex())
    {
    case 0:
        resultVisibility= Core::HIDDEN;
        break;
    case 1:
        resultVisibility= Core::FOGOFWAR;
        break;
    case 2:
        resultVisibility= Core::ALL;
        break;
    default:
        break;
    }
    return resultVisibility;
}

void MapWizzardDialog::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MapWizzardDialog::updateUI()
{
    /*ui->m_gridPattern->setCurrentIndex(map->getOption(VisualItem::GridPattern).toInt());
    ui->m_sizeGrid->setValue(map->getOption(VisualItem::GridSize).toInt());
    ui->m_gridColorBtn->setColor(map->getOption(VisualItem::GridColor).value<QColor>());
    ui->m_permissionComboBox->setCurrentIndex(map->getPermissionMode());
    ui->m_visibilityComboBox->setCurrentIndex(map->getOption(VisualItem::VisibilityMode).toInt());
    ui->m_scaleOfGrid->setValue(map->getOption(VisualItem::GridSize).toInt());
    ui->m_unitPattern->setCurrentIndex(map->getOption(VisualItem::Unit).toInt());*/
    // ui->m_colorButton->setColor(map->getBackGroundColor());

    // selectedShapeChanged();
    // ui->m_titleLineedit->setText(map->getMapTitle());
}
