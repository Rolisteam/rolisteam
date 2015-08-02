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



#include "vmapwizzarddialog.h"
#include "ui_vmapwizzarddialog.h"

#include "preferences/preferencesmanager.h"


MapWizzardDialog::MapWizzardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapWizzardDialog)
{
    ui->setupUi(this);
    m_model = new PatternModel();
    
    ui->m_gridPattern->setModel(m_model);
    
    
    m_options = PreferencesManager::getInstance();
    m_bgColor = QColor(255,255,255);
    
    m_bgColor = m_options->value(QString("MapWizzard/backgroundcolor"),QVariant::fromValue(m_bgColor)).value<QColor>();
    
    ui->m_colorButton->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(m_bgColor.red()).arg(m_bgColor.green()).arg(m_bgColor.blue()));
    connect(ui->m_colorButton,SIGNAL(clicked()),this,SLOT(clickOnColorButton()));
    ui->m_gridColorBtn->setColor(QColor(0,0,0));

    m_permissionData   << tr("No Right") << tr("His character") << tr("All Permissions");
    ui->m_permissionComboBox->addItems(m_permissionData);
    ui->m_permissionComboBox->setCurrentIndex(PreferencesManager::getInstance()->value("defaultPermissionMap",0).toInt());


    connect(ui->m_landscapeButton,SIGNAL(clicked()),this,SLOT(selectedShapeChanged()));
    connect(ui->m_portraitButton,SIGNAL(clicked()),this,SLOT(selectedShapeChanged()));
    connect(ui->m_squareButton,SIGNAL(clicked()),this,SLOT(selectedShapeChanged()));
    
    selectedShapeChanged();
    
}


MapWizzardDialog::~MapWizzardDialog()
{
    delete ui;
}

void MapWizzardDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void MapWizzardDialog::selectedShapeChanged()
{
    
    if(ui->m_landscapeButton->isChecked())
    {
        ui->m_smallSizeButton->setText(tr("Small ( %1 x %2)").arg(600).arg(450));
        ui->m_mediumSizeButton->setText(tr("Medium ( %1 x %2)").arg(800).arg(600));
        ui->m_bigSizeButton->setText(tr("Big ( %1 x %2)").arg(1000).arg(750));
        ui->m_hugeSizeButton->setText(tr("Huge ( %1 x %2)").arg(1200).arg(900));


    }
    else if(ui->m_portraitButton->isChecked())
    {
        ui->m_smallSizeButton->setText(tr("Small ( %2 x %1)").arg(600).arg(450));
        ui->m_mediumSizeButton->setText(tr("Medium ( %2 x %1)").arg(800).arg(600));
        ui->m_bigSizeButton->setText(tr("Big ( %2 x %1)").arg(1000).arg(750));
        ui->m_hugeSizeButton->setText(tr("Huge ( %2 x %1)").arg(1200).arg(900));
    }
    else if(ui->m_squareButton->isChecked())
    {
        ui->m_smallSizeButton->setText(tr("Small ( %2 x %1)").arg(500).arg(500));
        ui->m_mediumSizeButton->setText(tr("Medium ( %2 x %1)").arg(700).arg(700));
        ui->m_bigSizeButton->setText(tr("Big ( %2 x %1)").arg(900).arg(900));
        ui->m_hugeSizeButton->setText(tr("Huge ( %2 x %1)").arg(1100).arg(1100));
    }
    
    
}
void MapWizzardDialog::setAllMap(VMap* map)
{
    VMap::GRID_PATTERN grid;
    switch (ui->m_gridPattern->currentIndex())
    {
    case 0:
        grid = VMap::NONE;
        break;
    case 1:
        grid = VMap::SQUARE;
        break;
    case 3:
        grid = VMap::OCTOGON;
        break;
    case 2:
        grid = VMap::HEXAGON;
        break;
    default:
        grid = VMap::NONE;
        break;
    }
    map->setPattern(grid);
    map->setPatternSize(ui->m_sizeGrid->value());
    map->setPatternColor(ui->m_gridColorBtn->color());

    Map::PermissionMode result;
    switch (ui->m_permissionComboBox->currentIndex())
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

    map->setPermissionMode(result);
    map->setScale(ui->m_scaleOfGrid->value());
    map->setScaleUnit(ui->m_unitPattern->currentIndex());
    map->setBackGroundColor(m_bgColor);
    map->computePattern();

    if(ui->m_customSize->isChecked())
    {
        bool ok = false;

        int w = ui->m_widthEdit->text().toInt(&ok);
        if(ok)
            map->setWidth(w);
        int h = ui->m_heightEdit->text().toInt(&ok);
        if(ok)
            map->setHeight(h);
    }
    else
    {
        if(ui->m_landscapeButton->isChecked())
        {
            if(ui->m_smallSizeButton->isChecked())
            {
                map->setWidth(600);
                map->setHeight(450);

            }
            else if(ui->m_mediumSizeButton->isChecked())
            {
                map->setWidth(800);
                map->setHeight(600);

            }
            else if(ui->m_bigSizeButton->isChecked())
            {
                map->setWidth(1000);
                map->setHeight(750);

            }
            else if(ui->m_hugeSizeButton->isChecked())
            {
                map->setWidth(1200);
                map->setHeight(900);

            }
        }
        else if(ui->m_portraitButton->isChecked())
        {
            if(ui->m_smallSizeButton->isChecked())
            {
                map->setWidth(450);
                map->setHeight(600);

            }
            else if(ui->m_mediumSizeButton->isChecked())
            {
                map->setWidth(600);
                map->setHeight(800);

            }
            else if(ui->m_bigSizeButton->isChecked())
            {
                map->setWidth(750);
                map->setHeight(1000);

            }
            else if(ui->m_hugeSizeButton->isChecked())
            {
                map->setWidth(900);
                map->setHeight(1200);
            }
        }
        else if(ui->m_squareButton->isChecked())
        {
            if(ui->m_smallSizeButton->isChecked())
            {
                map->setWidth(500);
                map->setHeight(500);

            }
            else if(ui->m_mediumSizeButton->isChecked())
            {
                map->setWidth(700);
                map->setHeight(700);

            }
            else if(ui->m_bigSizeButton->isChecked())
            {
                map->setWidth(900);
                map->setHeight(900);

            }
            else if(ui->m_hugeSizeButton->isChecked())
            {
                map->setWidth(1100);
                map->setHeight(1100);
            }

        }

    }
    map->setTitle(ui->m_titleLineedit->text());
}

void MapWizzardDialog::clickOnColorButton()
{
    m_bgColor = QColorDialog::getColor ( m_bgColor, this );
    ui->m_colorButton->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(m_bgColor.red()).arg(m_bgColor.green()).arg(m_bgColor.blue()));
    m_options->registerValue("MapWizzard/backgroundcolor",m_bgColor);
}
