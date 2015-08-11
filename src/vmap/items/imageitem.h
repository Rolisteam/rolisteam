#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "visualitem.h"
#include <QImage>

class ImageItem : public VisualItem
{
public:
	ImageItem();
	/**
	* @brief paint the current rectangle into the scene.
	* @see Qt documentation
	*/
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

	/**
	* @brief gives bounding rect. Return rect geometry into the QRectF
	*/
	virtual QRectF boundingRect() const ;

	/**
	* @brief defines new end point.
	*/
	virtual void setNewEnd(QPointF& nend);
	/**
	* @brief serialization function to write data.
	*/
	virtual void writeData(QDataStream& out) const;
	/**
	* @brief serialization function to read data from stream.
	*/
	virtual void readData(QDataStream& in);
	/**
	 * @brief getType
	 * @return
	 */
	virtual VisualItem::ItemType getType();
	/**
	 * @brief fillMessage
	 * @param msg
	 */
	virtual void fillMessage(NetworkMessageWriter* msg);
	/**
	 * @brief readItem
	 * @param msg
	 */
	virtual void readItem(NetworkMessageReader* msg);

	/**
	 * @brief setGeometryPoint
	 * @param pointId
	 * @param pos
	 */
	virtual void setGeometryPoint(qreal pointId, QPointF &pos);
	/**
	 * @brief initChildPointItem
	 */
	virtual void initChildPointItem();

	void setImageUri(QString uri);
	/**
	 * @brief getImageUri
	 * @return
	 */
	QString getImageUri();


protected:
	/**
	 * @brief updateChildPosition
	 */
	virtual void updateChildPosition();

private:
	void loadImage();
private:
	bool m_keepAspect;///< flag to keep the aspect.
	QImage m_image;
	QString m_imagePath;
	qreal m_ratio;

};

#endif // IMAGEITEM_H
