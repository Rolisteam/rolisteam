/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "colordelegate.h"

#include "customs/colorlisteditor.h"

namespace rwidgets
{
ColorDelegate::ColorDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget* ColorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new ColorListEditor(parent);
}

void ColorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(!index.isValid())
        return;
    ColorListEditor* cb= qobject_cast<ColorListEditor*>(editor);
    QColor checked= index.data().value<QColor>();
    cb->setColor(checked);
}

void ColorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(!index.isValid())
        return;

    ColorListEditor* cb= qobject_cast<ColorListEditor*>(editor);
    if(nullptr != cb)
    {
        QColor color= cb->color();
        model->setData(index, color);
    }
}
} // namespace rwidgets
