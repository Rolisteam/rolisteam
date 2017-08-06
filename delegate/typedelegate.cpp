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
#include "typedelegate.h"
#include <QComboBox>

TypeDelegate::TypeDelegate()
{
    m_data << tr("TextInput")
           << tr("TextField")
           << tr("TextArea")
           << tr("Select")
           << tr("Checkbox")
           << tr("Image")
           << tr("Dice Button")
           << tr("Type Button");
}

QWidget* TypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // ComboBox ony in column 2
    if (index.column() != 4)
        return QStyledItemDelegate::createEditor(parent, option, index);


    QComboBox* cm = new QComboBox(parent);
    for(QString item : m_data)
    {
        cm->addItem(item,m_data.indexOf(item));
    }
    return cm;
}
void TypeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor))
    {
       int currentIndex = index.data(Qt::EditRole).toInt();
       if (currentIndex >= 0)
       {
           cb->setCurrentIndex(currentIndex);
       }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
void TypeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor))
    {
        model->setData(index, cb->currentIndex(), Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

QString TypeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    bool b;
    int i = value.toInt(&b);
    if((b)&&(i>=0)&&(i<m_data.size()))
    {
        return m_data.at(i);
    }
    return 0;
}
