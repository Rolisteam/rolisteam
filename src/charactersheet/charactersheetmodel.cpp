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

#include "charactersheetmodel.h"
#include "charactersheet.h"

CharacterSheetModel::CharacterSheetModel()
{
    m_characterList = new QList<CharacterSheet*>;
}

int CharacterSheetModel::rowCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent)
    int max = 0;
    for(int i = 0;i<m_characterList->size();i++)
    {
        max += m_characterList->at(0)->getIndexCount();
    }
    return max;
}
int CharacterSheetModel::columnCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent)
    return m_characterList->size()+1;
}
QModelIndex CharacterSheetModel::index ( int row, int column, const QModelIndex & parent ) const
{
    return parent.child(row,column);
}
QModelIndex CharacterSheetModel::parent ( const QModelIndex & index ) const
{
        Q_UNUSED(index)
    return QModelIndex();
}

QVariant CharacterSheetModel::data ( const QModelIndex & index, int role  ) const
{
    Q_UNUSED(index)
    if(role == Qt::DisplayRole)
        return QString("merde");

    return QVariant();
}
bool CharacterSheetModel::setData ( const QModelIndex & index, const QVariant & value, int role  )
{
    if(Qt::EditRole==role)
    {
        if(index.column()!=0)
        {
            CharacterSheet* tmp = m_characterList->at(index.column()-1);
            tmp->setData(index.row(),value);
        }
        else
        {
            CharacterSheet* tmp = m_characterList->at(index.column()-1);
            tmp->setData(index.row(),0,true);
        }
    }
}
