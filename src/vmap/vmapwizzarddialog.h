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
#ifndef MAPWIZZARDDIALOG_H
#define MAPWIZZARDDIALOG_H

#include <QDialog>
#include <QColor>

#include "vmap.h"
#include "patternmodel.h"

namespace Ui {
class MapWizzardDialog;
}


class PreferencesManager;
/**
    * @brief set values to create a map with appropriate value.
    */
class MapWizzardDialog : public QDialog {
    Q_OBJECT
public:
    /**
    * @brief constructor
    * @param parent, pointer to the parent widget.
    */
    MapWizzardDialog(QWidget *parent = 0);
    /**
    * @brief destructor
    */
    ~MapWizzardDialog();
    
    
    /**
    * @brief Define all paramaters of the map.
    * @param map must be fully defined.
    */
    void setAllMap(VMap* map);
    
protected:
    /**
    * @brief overwritting the changeevent: create by Qtcreator
    * @param event describe the context of the event.
    */
    void changeEvent(QEvent *e);
    
private slots:
    /**
    * @brief slot called when user changed the shape of the future map.
    */
    void selectedShapeChanged();
    /**
    * @brief slot called when user click on the color button, start a colorselector dialog.
    */
    void clickOnColorButton();
    
private:
    /**
    * @brief pointer to the user interface Qt designer
    */
    Ui::MapWizzardDialog *ui;
    /**
    * @brief color used for painting the background
    */
    QColor m_bgColor;
    
    /**
    * @brief pointer to the unique instance of preference manager.
    */
    PreferencesManager* m_options;
    
    /**
    * @brief model gathering all grid patterns.
    */
    PatternModel* m_model;
};

#endif // MAPWIZZARDDIALOG_H
