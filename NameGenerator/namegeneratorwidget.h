/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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

#ifndef NAMEGENERATORWIDGET_H
#define NAMEGENERATORWIDGET_H

#include <QWidget>
#include "widgets/gmtoolbox/gamemastertool.h"

namespace Ui {
class NameGeneratorWidget;
}


struct DataBase;
/**
 * @page NameGenerator Name Generator
 * @tableofcontents
 * @section Intro Introduction
 * NameGenerator provides a way to get ramdom name from different culture.<br/>
 * This tool is dedicated to GM.<br/>
 *
 *
 * @section algo Let's have a look to the algo:
 *
 * @section todo Todo:
 * @todo Add new country
 */

/**
 * @brief The NameGeneratorWidget class
 */
class NameGeneratorWidget : public QWidget, public GameMasterTool
{
    Q_OBJECT

public:
    enum AVAILABLE_GENDER{Female,Male,BOTH,NONE};
    enum TypeOfGeneration{Chinese,Elves,English,French,Japanese,StarWars,Russian};

    explicit NameGeneratorWidget(QWidget *parent = 0);
    ~NameGeneratorWidget();
protected:
    void buildAllNames(int count, QHash<QString, DataBase> data);
protected slots:
    void checkFeatureAvailability();
private slots:
    void generateName();
    bool nextCharacterCanEnd(const QJsonObject& json,QString key);
    bool nextIsPossible(const QJsonObject& json,QString key,bool last);


private:
    QString buildName(const QJsonObject &json);
    QString pickUpName(QStringList data);


private:
    Ui::NameGeneratorWidget *ui;
    QStringList m_model;
    QHash<TypeOfGeneration,QHash<QString,DataBase> > m_complexName;
};

/**
 * @brief The DataBase struct stored data for database
 */
struct DataBase
{
    NameGeneratorWidget::AVAILABLE_GENDER gender;
    QString filepath;
    int id;
    bool generate;
};
#endif // NAMEGENERATORWIDGET_H
