#ifndef VISUALITEMCONTROLLER_H
#define VISUALITEMCONTROLLER_H

#include <QObject>
#include <QPointF>
#include <QPointer>

#include "media/mediatype.h"

class VectorialMapController;
class VisualItemController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(bool editable READ editable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(bool selectable READ selectable NOTIFY selectableChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(Core::Layer layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos NOTIFY posChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)

public:
    VisualItemController(VectorialMapController* ctrl, QObject* parent= nullptr);
    ~VisualItemController();

    bool selected() const;
    bool editable() const;
    bool selectable() const;
    bool visible() const;
    qreal opacity() const;
    Core::Layer layer() const;
    QPointF pos() const;
    QString uuid() const;
    qreal rotation() const;

    int gridSize() const;
    QString getLayerText(Core::Layer layer) const;

    virtual void aboutToBeRemoved()= 0;
    virtual void setCorner(const QPointF& move, int corner)= 0;

    virtual void endGeometryChange()= 0;

signals:
    void selectedChanged();
    void editableChanged();
    void selectableChanged();
    void visibleChanged();
    void opacityChanged();
    void layerChanged();
    void posChanged();
    void uuidChanged();
    void removeItem();
    void rotationChanged();

public slots:
    void setSelected(bool b);
    void setEditable(bool b);
    void setVisible(bool b);
    void setOpacity(qreal b);
    void setLayer(Core::Layer layer);
    void setPos(QPointF pos);
    void setUuid(QString uuid);
    void setRotation(qreal rota);

protected:
    QPointer<VectorialMapController> m_ctrl;
    bool m_selected= false;
    bool m_editable= true;
    bool m_visible= true;
    qreal m_opacity= 1.0;
    qreal m_rotation= 0.0;
    QPointF m_pos;
    Core::Layer m_layer= Core::Layer::NONE;
    QString m_uuid;
};

#endif // VISUALITEMCONTROLLER_H
