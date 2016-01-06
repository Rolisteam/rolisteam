/***************************************************************************
*	Copyright (C) 2015 by Renaud Guezennec                                *
*   http://renaudguezennec.homelinux.org/accueil,3.html                   *
*                                                                         *
*   Rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef PALETTEMODEL_H
#define PALETTEMODEL_H

#include <QAbstractTableModel>
#include <QJsonObject>


#include <QPalette>
#include <QColor>
/**
 * @brief The PaletteColor class
 */
class PaletteColor
{

public:
    PaletteColor(QColor,QString,QPalette::ColorGroup group,QPalette::ColorRole role);
    void setColor(QColor);
    void setName(QString);

    QColor getColor() const;
    QString getName() const;
    QPalette::ColorGroup getGroup();
    QPalette::ColorRole getRole();
    void writeTo(QJsonObject& json);
    bool readFrom(QJsonObject& json);
private:
    QColor m_color;
    QString m_name;
    QPalette::ColorGroup m_group;
    QPalette::ColorRole m_role;
};
/**
 * @brief The PaletteModel class
 */
class PaletteModel : public QAbstractTableModel
{
        Q_OBJECT
public:

    PaletteModel();
    virtual ~PaletteModel();

    void initData();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;



    void setPalette(QPalette);
    QPalette getPalette();
    void writeTo(QJsonObject& json);
    bool readFrom(const QJsonObject& json);
    void setColor(const QModelIndex &, QColor);
private:
    QList<PaletteColor*> m_data;
    QStringList m_header;
    QStringList m_groupList;

};

#endif // PALETTEMODEL_H
