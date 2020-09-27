/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef NODESTYLE_H
#define NODESTYLE_H

#include <QColor>
#include <QObject>

class NodeStyle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor colorOne READ colorOne WRITE setColorOne NOTIFY colorOneChanged)
    Q_PROPERTY(QColor colorTwo READ colorTwo WRITE setColorTwo NOTIFY colorTwoChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
public:
    explicit NodeStyle(QObject* parent= nullptr);

    QColor colorOne() const;
    QColor colorTwo() const;
    QColor textColor() const;
signals:
    void colorOneChanged();
    void colorTwoChanged();
    void textColorChanged();

public slots:
    void setColorOne(const QColor& color);
    void setColorTwo(const QColor& color);
    void setTextColor(const QColor& color);

private:
    QColor m_colorOne;
    QColor m_colorTwo;
    QColor m_textColor;
};

#endif // NODESTYLE_H
