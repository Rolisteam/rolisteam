/***************************************************************************
 *   Copyright (C) 2016 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                                      *
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
#include "alignmentdelegate.h"
#include "charactersheetitem.h"
#include <QComboBox>

AlignmentDelegate::AlignmentDelegate(QWidget* parent) : QStyledItemDelegate(parent) {}

QWidget* AlignmentDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const
{
    // ComboBox ony in column 2
    if(index.column() != static_cast<int>(CharacterSheetItem::TEXT_ALIGN))
        return QStyledItemDelegate::createEditor(parent, option, index);

    QStringList data;
    data << tr("TopRight") << tr("TopMiddle") << tr("TopLeft") << tr("CenterRight") << tr("CenterMiddle")
         << tr("CenterLeft") << tr("BottomRight") << tr("BottomMiddle") << tr("BottomLeft");

    QComboBox* cm= new QComboBox(parent);
    cm->addItems(data);
    return cm;
}
void AlignmentDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(QComboBox* cb= qobject_cast<QComboBox*>(editor))
    {
        int currentIndex= index.data(Qt::EditRole).toInt();
        if(currentIndex >= 0)
        {
            cb->setCurrentIndex(currentIndex);
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
void AlignmentDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(QComboBox* cb= qobject_cast<QComboBox*>(editor))
    {
        model->setData(index, cb->currentIndex(), Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}
