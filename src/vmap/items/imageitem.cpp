#include "imageitem.h"

#include <QPainter>
#include <QDebug>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

ImageItem::ImageItem()
: VisualItem()
{
	m_keepAspect = true;
}
QRectF ImageItem::boundingRect() const
{
	return m_rect;
}
void ImageItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	painter->save();
	painter->drawImage(m_rect,m_image,m_image.rect());

	if(hasFocusOrChild())
	{
		if(NULL!=m_child)
		{
			foreach(ChildPointItem* item, *m_child)
			{
				item->setVisible(true);
			}
		}
	}
	else
	{
		if(NULL!=m_child)
		{
			foreach(ChildPointItem* item, *m_child)
			{
				item->setVisible(false);
			}
		}
	}
	painter->restore();

}
void ImageItem::setNewEnd(QPointF& p)
{
	m_rect.setBottomRight(p);
}

VisualItem::ItemType ImageItem::getType()
{
	return VisualItem::IMAGE;
}
void ImageItem::writeData(QDataStream& out) const
{
	out << m_rect;
	out << m_keepAspect;
	out << m_color;
	out << m_id;
}

void ImageItem::readData(QDataStream& in)
{
	in >> m_rect;
	in >> m_keepAspect;
	in >> m_color;
	in >> m_id;
}
void ImageItem::fillMessage(NetworkMessageWriter* msg)
{
	msg->string16(m_id);

	//rect
	msg->real(m_rect.x());
	msg->real(m_rect.y());
	msg->real(m_rect.width());
	msg->real(m_rect.height());

	msg->int8(m_keepAspect);
	msg->rgb(m_color);



	msg->real(scale());
	msg->real(rotation());

}
void ImageItem::readItem(NetworkMessageReader* msg)
{
	m_id = msg->string16();
	//rect
	m_rect.setX(msg->real());
	m_rect.setY(msg->real());
	m_rect.setWidth(msg->real());
	m_rect.setHeight(msg->real());
	m_keepAspect = msg->int8();
	m_color = msg->rgb();

	setTransformOriginPoint(m_rect.center());
	setScale(msg->real());
	setRotation(msg->real());
}
void ImageItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
	switch ((int)pointId)
	{
	case 0:
		m_rect.setTopLeft(pos);
		m_child->value(1)->setPos(m_rect.topRight());
		m_child->value(2)->setPos(m_rect.bottomRight());
		m_child->value(3)->setPos(m_rect.bottomLeft());
		break;
	case 1:
		m_rect.setTopRight(pos);
		 m_child->value(0)->setPos(m_rect.topLeft());
		m_child->value(2)->setPos(m_rect.bottomRight());
		m_child->value(3)->setPos(m_rect.bottomLeft());
		break;
	case 2:
		m_rect.setBottomRight(pos);
		m_child->value(0)->setPos(m_rect.topLeft());
		m_child->value(1)->setPos(m_rect.topRight());
		m_child->value(3)->setPos(m_rect.bottomLeft());
		break;
	case 3:
		m_rect.setBottomLeft(pos);
		m_child->value(0)->setPos(m_rect.topLeft());
		m_child->value(1)->setPos(m_rect.topRight());
		m_child->value(2)->setPos(m_rect.bottomRight());
		break;
	default:
		break;
	}

	setTransformOriginPoint(m_rect.center());
	//updateChildPosition();
}
void ImageItem::resizeContents(const QRect& rect, bool keepRatio)
{
    if (!rect.isValid())
        return;

    prepareGeometryChange();

    m_rect = rect.normalized();
    if (keepRatio)
    {
        int width = m_image.width();
        int height = m_image.height();
        qreal hfw = height * rect.width() / width;
        if (hfw > 1)
        {
            m_rect.setTop(-hfw / 2);
            m_rect.setHeight(hfw);
        }
    }

    updateChildPosition();
}
void ImageItem::initChildPointItem()
{
	setPos(m_rect.center());
	m_rect.setCoords(-m_rect.width()/2,-m_rect.height()/2,m_rect.width()/2,m_rect.height()/2);
	setTransformOriginPoint(m_rect.center());

	m_rect = m_rect.normalized();
	setTransformOriginPoint(m_rect.center());
    if((NULL == m_child))
    {
        m_child = new QVector<ChildPointItem*>();
    }
    else
    {
        m_child->clear();
    }


	for(int i = 0; i< 4 ; ++i)
	{
		ChildPointItem* tmp = new ChildPointItem(i,this);
		tmp->setMotion(ChildPointItem::MOUSE);
		m_child->append(tmp);
	}
   updateChildPosition();
}
void ImageItem::updateChildPosition()
{
    if((NULL!= m_child) && (!m_child->isEmpty()))
    {
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(0)->setPlacement(ChildPointItem::TopLeft);
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(1)->setPlacement(ChildPointItem::TopRight);
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(2)->setPlacement(ChildPointItem::ButtomRight);
        m_child->value(3)->setPos(m_rect.bottomLeft());
        m_child->value(3)->setPlacement(ChildPointItem::ButtomLeft);
    }
	setTransformOriginPoint(m_rect.center());

	update();
}
void ImageItem::setImageUri(QString uri)
{
	m_imagePath = uri;
	loadImage();
}

QString ImageItem::getImageUri()
{
	return m_imagePath;
}
void ImageItem::loadImage()
{
	m_image.load(m_imagePath);
	m_rect = m_image.rect();
    m_ratio = m_image.height()/m_image.width();
}
void ImageItem::setModifiers(Qt::KeyboardModifiers modifiers)
{
    m_modifiers = modifiers;
}
VisualItem* ImageItem::getItemCopy()
{
	ImageItem* rectItem = new ImageItem();
	rectItem->setImageUri(m_imagePath);
	rectItem->resizeContents(m_rect.toRect(),false);
    rectItem->setPos(pos());
	return rectItem;
}
