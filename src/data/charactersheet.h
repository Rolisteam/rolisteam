/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef CHARACTERSHEET_H
#define CHARACTERSHEET_H
#include <QString>
#include <QMap>
#include <QVariant>
//typedef QMap<QString,QString> Section;
class Section : public QMap<QString,QString>
{
public:
    Section();


    QString getName();
    void setName(QString name);

private:
    QString m_name;


};

class CharacterSheet
{
public:
    CharacterSheet();
    const QString getData(int section,const QString& key);
    const QString getTitleSection(int section);


    const  QString getData(int index);
    const QString getkey(int index);

    int getSection(int index);

    void setData(int index,QVariant value,bool isHeader = false);
    int getIndexCount();

private:
    QList<Section> m_sectionList;
    QString m_owner;
};

#endif // CHARACTERSHEET_H
