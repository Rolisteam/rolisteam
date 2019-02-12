/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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

#include <QImage>
#include <QPainter>

#include "patternmodel.h"

//////IMPLEMENTATION of PatternModel
PatternModel::PatternModel()
{
    m_list.append(QPixmap());
    m_list.append(QPixmap(":/grid/resources/gridpattern/01_sqare.png"));
    m_list.append(QPixmap(":/grid/resources/gridpattern/03_hexa.png"));
    // m_list.append(QPixmap(":/grid/resources/gridpattern/02_octo.png"));

    m_listGrille << tr("NoGrid") << tr("Square") << tr("Hexagon") /*<< tr("Octogon")*/;
}
QVariant PatternModel::data(const QModelIndex& index, int role) const
{
    if(Qt::DisplayRole == role)
    {
        QVariant variant= m_listGrille.at(index.row());
        return variant;
    }
    else if(role == Qt::DecorationRole)
    {
        QVariant variant= m_list.at(index.row());
        return variant;
    }
    else
    {
        return QVariant();
    }
}
int PatternModel::rowCount(const QModelIndex&) const
{
    return m_list.size();
}

QPixmap PatternModel::getPatternAt(int i)
{
    return m_list.at(i);
}
