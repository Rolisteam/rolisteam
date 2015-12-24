/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

namespace Ui {
class NameGeneratorWidget;
}


struct DataBase;

class NameGeneratorWidget : public QWidget
{
    Q_OBJECT

public:
    enum AVAILABLE_GENDER{Female,Male,BOTH};
    explicit NameGeneratorWidget(QWidget *parent = 0);
    ~NameGeneratorWidget();
private slots:
    void generateName();
private:
    QString buildName(const QJsonObject &json,int len);


private:
    Ui::NameGeneratorWidget *ui;
    QStringList m_model;
    QList<DataBase> m_gender;
};


struct DataBase
{
    NameGeneratorWidget::AVAILABLE_GENDER gender;
    QString filepath;
    int id;
};
#endif // NAMEGENERATORWIDGET_H
