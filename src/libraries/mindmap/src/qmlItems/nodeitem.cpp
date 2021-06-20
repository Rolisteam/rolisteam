#include "nodeitem.h"

#include "geometry/boxnode.h"

#define RADIUS 10.f

NodeItem::NodeItem(QQuickItem* parent) : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(static_cast<Qt::MouseButtons>(Qt::AllButtons));
    setFlag(ItemAcceptsInputMethod, true);
}

QString NodeItem::text()
{
    return m_text;
}

void NodeItem::setText(const QString& text)
{
    if(text == m_text)
        return;
    m_text= text;
    emit textChanged();
}
QColor NodeItem::color() const
{
    return m_color;
}
void NodeItem::setColor(const QColor& color)
{
    if(color == m_color)
        return;
    m_color= color;
    emit colorChanged();
}
QPointF NodeItem::position() const
{
    return m_position;
}
void NodeItem::setPosition(const QPointF& position)
{
    if(position == m_position)
        return;
    m_position= position;
    emit positionChanged();
}

QSGNode* NodeItem::updatePaintNode(QSGNode* node, UpdatePaintNodeData*)
{
    BoxNode* boxNode= static_cast<BoxNode*>(node);
    if(nullptr == boxNode)
    {
        boxNode= new BoxNode();
        boxNode->setColor(m_color);
        // auto pos= boundingRect().topLeft();
        qDebug() << boundingRect() << m_color;

        boxNode->update(boundingRect() /**/, RADIUS);
    }
    return boxNode;
}

void NodeItem::mousePressEvent(QMouseEvent* event)
{
    QQuickItem::mousePressEvent(event);
}
void NodeItem::mouseMoveEvent(QMouseEvent* event)
{
    QQuickItem::mouseMoveEvent(event);
}
