#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "visualitem.h"
#include <QImage>
#include <QMovie>

class ImageItem : public VisualItem
{
    Q_OBJECT
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
	virtual VisualItem::ItemType getType() const;
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

    virtual void setModifiers(Qt::KeyboardModifiers modifiers);

    /**
     * @brief resizeContents
     * @param rect
     * @param keepRatio
     */
    virtual void resizeContents(const QRectF& rect, bool keepRatio = true);
	/**
	 * @brief getItemCopy
	 * @return
	 */
	virtual VisualItem* getItemCopy();



    VisualItem* promoteTo(VisualItem::ItemType);
protected slots:
    void updateImageFromMovie(QRect);

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
    Qt::KeyboardModifiers m_modifiers;
    bool m_initialized;

    //QAction* m_
    QMovie* m_movie;
};

#endif // IMAGEITEM_H
