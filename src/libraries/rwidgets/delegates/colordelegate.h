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
#ifndef RWIDGETS_COLORDELEGATE_H
#define RWIDGETS_COLORDELEGATE_H

#include "rwidgets_global.h"
#include <QStyledItemDelegate>
namespace rwidgets
{

/**
 * @brief The ColorDelegate class
 */
class RWIDGET_EXPORT ColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /**
     * @brief CheckBoxDelegate
     * @param aRedCheckBox
     * @param parent
     */
    ColorDelegate(QObject* parent= nullptr);
    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    /**
     * @brief setModelData
     * @param editor
     * @param model
     * @param index
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
};
} // namespace rwidgets
#endif // RWIDGETS_COLORDELEGATE_H
