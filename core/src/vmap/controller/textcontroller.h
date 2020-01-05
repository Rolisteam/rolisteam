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
#ifndef TEXTITEMCONTROLLER_H
#define TEXTITEMCONTROLLER_H

#include "visualitemcontroller.h"

#include <QColor>
#include <QFont>
#include <QRectF>
#include <QVariant>
#include <vector>

class VectorialMapController;
namespace vmap
{
class TextController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QRectF textRect READ textRect NOTIFY textRectChanged)
    Q_PROPERTY(QRectF borderRect READ borderRect WRITE setBorderRect NOTIFY borderRectChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool border READ border NOTIFY borderChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(quint16 penWidth READ penWidth CONSTANT)
    Q_PROPERTY(QPointF textPos READ textPos WRITE setTextPos NOTIFY textPosChanged)
public:
    TextController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent= nullptr);

    QString text() const;
    QRectF textRect() const;
    QRectF borderRect() const;
    QColor color() const;
    bool border() const;
    QFont font() const;
    QPointF textPos() const;
    quint16 penWidth() const;

    void aboutToBeRemoved() override;
    void endGeometryChange() override;
    QRectF rect() const override;

public slots:
    void setText(QString text);
    void setColor(QColor color);
    void setBorderRect(QRectF rect);
    void setTextRect(QRectF rect);
    void setCorner(const QPointF& move, int corner) override;
    void setFont(QFont font);
    void setTextPos(QPointF textPos);

    void increaseFontSize();
    void decreaseFontSize();

signals:
    void textChanged(QString text);
    void textRectChanged(QRectF textRect);
    void borderRectChanged(QRectF borderRect);
    void colorChanged(QColor color);
    void borderChanged(bool border);
    void fontChanged(QFont font);

    void textPosChanged(QPointF textPos);

private:
    void updateSize();
    void updateTextPosition();

private:
    QString m_text;
    QRectF m_textRect;
    QPointF m_textPos;
    QRectF m_borderRect;
    QColor m_color;
    bool m_border;
    QFont m_font;
    quint16 m_penWidth;
};
} // namespace vmap

#endif // TEXTITEMCONTROLLER_H
