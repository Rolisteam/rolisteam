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
#ifndef RWIDGETS_COLORLISTEDITOR_H
#define RWIDGETS_COLORLISTEDITOR_H

#include "rwidgets_global.h"
#include <QComboBox>
namespace rwidgets
{
/**
 * @brief The ColorListEditor class
 */
class RWIDGET_EXPORT ColorListEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true)

public:
    ColorListEditor(QWidget* widget= nullptr);

    QColor color() const;
    void setColor(QColor c);

signals:
    void colorChanged();

private:
    void populateList();
};
} // namespace rwidgets
#endif // RWIDGETS_COLORLISTEDITOR_H
