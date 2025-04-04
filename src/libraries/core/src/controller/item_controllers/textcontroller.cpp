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
#include "controller/item_controllers/textcontroller.h"

#include "worker/utilshelper.h"
#include <QFontMetrics>

#define MARGIN 6

bool greater(const QRectF& r1, const QRectF& r2)
{
    return r1.width() * r1.height() > r2.width() * r2.height();
}

namespace vmap
{
TextController::TextController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::TEXT, params, ctrl, parent)
{
    namespace hu= helper::utils;
    namespace cv= Core::vmapkeys;
    using std::placeholders::_1;

    if(params.end() != params.find("color"))
        setColor(params.at(QStringLiteral("color")).value<QColor>());

    hu::setParamIfAny<Core::SelectableTool>(cv::KEY_TOOL, params, [this](Core::SelectableTool tool) { m_tool= tool; });

    hu::setParamIfAny<bool>(cv::KEY_BORDER, params, [this](bool hasBorder) { m_border= hasBorder; });

    if(m_border && (m_tool == Core::SelectableTool::TEXTBORDER))
        qWarning() << "wrong data about border on text item.";

    hu::setParamIfAny<QString>(cv::KEY_TEXT, params, std::bind(&TextController::setText, this, _1));
    hu::setParamIfAny<QPointF>(cv::KEY_TEXTPOS, params, std::bind(&TextController::setTextPos, this, _1));
    hu::setParamIfAny<QRectF>(cv::KEY_TEXTRECT, params, std::bind(&TextController::setTextRect, this, _1));
    hu::setParamIfAny<QRectF>(cv::KEY_BORDERRECT, params, std::bind(&TextController::setBorderRect, this, _1));
    hu::setParamIfAny<quint16>(cv::KEY_PENWIDTH, params, [this](quint16 pen) { m_penWidth= pen; });

    if(m_text.isEmpty())
        setText(tr("Text"));

    connect(this, &TextController::textChanged, this, [this] { setModified(); });
    connect(this, &TextController::textRectChanged, this, [this] { setModified(); });
    connect(this, &TextController::borderRectChanged, this, [this] { setModified(); });
    connect(this, &TextController::borderChanged, this, [this] { setModified(); });
    connect(this, &TextController::fontChanged, this, [this] { setModified(); });
    connect(this, &TextController::textPosChanged, this, [this] { setModified(); });
}
QString TextController::text() const
{
    return m_text;
}
void TextController::setText(QString text)
{
    if(m_text == text)
        return;

    m_text= text;
    emit textChanged(m_text);
}

QRectF TextController::textRect() const
{
    return m_textRect;
}

QRectF TextController::borderRect() const
{
    return m_borderRect;
}

bool TextController::border() const
{
    return m_border;
}
void TextController::aboutToBeRemoved()
{
    emit removeItem();
}

void TextController::setBorderRect(QRectF rect)
{
    if(m_borderRect == rect || !greater(rect, textRect()))
        return;

    m_borderRect= rect;
    emit borderRectChanged(m_borderRect);
    m_editingBorderRect= true;
}

void TextController::setTextRect(QRectF rect)
{
    if(m_textRect == rect)
        return;

    m_textRect= rect;
    emit textRectChanged(m_textRect);
    updateSize();
    updateTextPosition();
}
QFont TextController::font() const
{
    return m_font;
}
void TextController::setFont(QFont font)
{
    if(m_font == font)
        return;

    m_font= font;
    emit fontChanged(m_font);
}

void TextController::updateSize()
{
    if(greater(textRect(), borderRect()))
    {
        setBorderRect(textRect().adjusted(0, 0, 0, MARGIN));
    }
}

void TextController::increaseFontSize()
{
    m_font.setPointSize(m_font.pointSize() + 1);
    updateSize();
}

void TextController::decreaseFontSize()
{
    if(m_font.pointSize() < 5)
        return;
    m_font.setPointSize(m_font.pointSize() - 1);
    updateSize();
}

void TextController::endGeometryChange()
{

    VisualItemController::endGeometryChange();
    if(m_editingBorderRect)
    {
        m_editingBorderRect= false;
        emit borderRectEditFinished();
    }
}

QRectF TextController::rect() const
{
    return borderRect();
}

void TextController::setTextPos(QPointF textPos)
{
    if(m_textPos == textPos)
        return;

    m_textPos= textPos;
    emit textPosChanged(m_textPos);
}
QPointF TextController::textPos() const
{
    return m_textPos;
}

quint16 TextController::penWidth() const
{
    return m_penWidth;
}

void TextController::updateTextPosition()
{
    setTextPos(QPointF(m_borderRect.x(), m_borderRect.center().y() - textRect().height() / 2));
}

void TextController::setCorner(const QPointF& move, int corner, Core::TransformType tt)
{
    Q_UNUSED(tt)
    if(move.isNull())
        return;

    auto rect= m_borderRect;
    qreal x2= rect.right();
    qreal y2= rect.bottom();
    qreal x= rect.x();
    qreal y= rect.y();
    switch(corner)
    {
    case TopLeft:
        x+= move.x();
        y+= move.y();
        break;
    case TopRight:
        x2+= move.x();
        y+= move.y();
        break;
    case BottomRight:
        x2+= move.x();
        y2+= move.y();
        break;
    case BottomLeft:
        x+= move.x();
        y2+= move.y();
        break;
    }
    rect.setCoords(x, y, x2, y2);
    if(!rect.isValid())
        rect= rect.normalized();
    setBorderRect(rect);

    // updateTextPosition();
}
} // namespace vmap
