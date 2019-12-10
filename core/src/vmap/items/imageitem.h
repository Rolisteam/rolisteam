#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "visualitem.h"
#include <QImage>
#include <QMovie>
namespace vmap
{
class ImageController;
}
class ImageItem : public VisualItem
{
    Q_OBJECT
public:
    ImageItem(vmap::ImageController* ctrl);
    /**
     * @brief paint the current rectangle into the scene.
     * @see Qt documentation
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);

    /**
     * @brief gives bounding rect. Return rect geometry into the QRectF
     */
    virtual QRectF boundingRect() const;

    /**
     * @brief defines new end point.
     */
    virtual void setNewEnd(const QPointF& nend);
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
    virtual void setGeometryPoint(qreal pointId, QPointF& pos);
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
     * @brief getItemCopy
     * @return
     */
    virtual VisualItem* getItemCopy();

    VisualItem* promoteTo(VisualItem::ItemType);
    QImage getImage() const;
    void setImage(const QImage& image);
    void updateImageFromMovie(QRect);

protected:
    /**
     * @brief updateChildPosition
     */
    virtual void updateChildPosition();

private:
    void loadImage();
    void dataToMedia();
    void initImage();

private:
    QPointer<vmap::ImageController> m_imgCtrl;
    bool m_keepAspect; ///< flag to keep the aspect.
    // QImage m_image;
    // QString m_imagePath;
    // qreal m_ratio;
    // Qt::KeyboardModifiers m_modifiers;
    // bool m_initialized;

    // QAction* m_
    // QMovie* m_movie;
    // QByteArray m_data;
};

#endif // IMAGEITEM_H
