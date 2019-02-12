/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
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
#ifndef USERLISTDELEGATE_H
#define USERLISTDELEGATE_H

#include <QStyledItemDelegate>
/**
 * @brief Overrides the list item painting
 * @todo Review it completely to make mouse click user-friendly
 * @author Renaud Guezennec
 */
class UserListDelegate : public QStyledItemDelegate
{
public:
    /**
     * @brief default construtor
     */
    explicit UserListDelegate(QObject* parent= nullptr);

protected:
    /**
     * @brief is called for painting each item
     */
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /**
     * @brief defines the size Hint of each item
     */
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

public slots:
};

#endif // USERLISTDELEGATE_H
