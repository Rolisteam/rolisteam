#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "rwidgets_global.h"
#include "visualitem.h"
#include <QImage>
#include <QMovie>
namespace vmap
{
class ImageItemController;
}
class RWIDGET_EXPORT ImageItem : public VisualItem
{
    Q_OBJECT
public:
    ImageItem(vmap::ImageItemController* ctrl);
    /**
     * @brief paint the current rectangle into the scene.
     * @see Qt documentation
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;

    /**
     * @brief gives bounding rect. Return rect geometry into the QRectF
     */
    virtual QRectF boundingRect() const override;

    /**
     * @brief defines new end point.
     */
    virtual void setNewEnd(const QPointF& nend) override;

    void setImageUri(QString uri);
    /**
     * @brief getImageUri
     * @return
     */
    QString getImageUri();

    virtual void setModifiers(Qt::KeyboardModifiers modifiers) override;

    VisualItem* promoteTo(vmap::VisualItemController::ItemType) override;
    QImage getImage() const;
    void setImage(const QImage& image);

    // void endOfGeometryChange(ChildPointItem::Change change) override;

protected:
    /**
     * @brief updateChildPosition
     */
    virtual void updateChildPosition() override;

private:
    void loadImage();
    void dataToMedia();
    void initImage();

private:
    QPointer<vmap::ImageItemController> m_imgCtrl;
    bool m_keepAspect; ///< flag to keep the aspect.
};

#endif // IMAGEITEM_H
