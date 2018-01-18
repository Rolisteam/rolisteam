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
#include "fontdelegate.h"
#include <QFontDialog>

FontDelegate::FontDelegate(QWidget* parent)
: QStyledItemDelegate(parent)
{

}

QWidget* FontDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // ComboBox ony in column 2
       if (index.column() != 10)
           return QStyledItemDelegate::createEditor(parent, option, index);


    QFontDialog* cm = new QFontDialog(parent);
    return cm;
}
void FontDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (QFontDialog* cb = qobject_cast<QFontDialog*>(editor))
    {
       QString fontStr = index.data(Qt::EditRole).toString();
       QFont font;
       font.fromString(fontStr);

       cb->setCurrentFont(font);

    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
void FontDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (QFontDialog* cb = qobject_cast<QFontDialog*>(editor))
    {
        model->setData(index, cb->currentFont().toString(), Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}
