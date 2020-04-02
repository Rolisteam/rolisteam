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
#ifndef VISUALITEMCONTROLLER_H
#define VISUALITEMCONTROLLER_H

#include <QColor>
#include <QObject>
#include <QPointF>
#include <QPointer>

#include "media/mediatype.h"

class VectorialMapController;
namespace vmap
{
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
    Q_PROPERTY(bool localIsGM READ localIsGM NOTIFY localIsGMChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(Core::SelectableTool tool READ tool CONSTANT)
    Q_PROPERTY(ItemType itemType READ itemType CONSTANT)
    Q_PROPERTY(bool initialized READ initialized WRITE setInitialized NOTIFY initializedChanged)

public:
    enum ItemType
    {
        PATH,
        LINE,
        ELLISPE,
        CHARACTER,
        TEXT,
        RECT,
        RULE,
        IMAGE,
        SIGHT,
        ANCHOR,
        GRID,
        HIGHLIGHTER
    };
    Q_ENUM(ItemType)
    enum Corner
    {
        TopLeft= 0,
        TopRight,
        BottomRight,
        BottomLeft,
    };
    Q_ENUM(Corner)
    VisualItemController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                         QObject* parent= nullptr);
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
    bool localIsGM() const;
    bool initialized() const;
    Core::SelectableTool tool() const;
    virtual QColor color() const;
    virtual QRectF rect() const= 0;
    virtual ItemType itemType() const= 0;

    const QString mapUuid() const;

    int gridSize() const;
    QString getLayerText(Core::Layer layer) const;

    virtual void aboutToBeRemoved()= 0;
    virtual void setCorner(const QPointF& move, int corner)= 0;

    virtual void endGeometryChange();

    bool locked() const;

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
    void localIsGMChanged();
    void colorChanged(QColor color);
    void initializedChanged(bool);
    void lockedChanged(bool locked);
    void posEditFinished();
    void rotationEditFinished();

public slots:
    void setSelected(bool b);
    void setEditable(bool b);
    void setVisible(bool b);
    void setOpacity(qreal b);
    void setLayer(Core::Layer layer);
    void setPos(const QPointF& pos);
    void setUuid(QString uuid);
    void setRotation(qreal rota);
    void setColor(const QColor& color);
    void setLocked(bool locked);
    void setInitialized(bool);

protected:
    virtual void computeEditable();

private:
    void initializedVisualItem(const std::map<QString, QVariant>& params);

protected:
    QPointer<VectorialMapController> m_ctrl;
    bool m_selected= false;
    bool m_editable= true;
    bool m_visible= true;
    bool m_locked= false;
    QColor m_color;
    qreal m_opacity= 1.0;
    qreal m_rotation= 0.0;
    QPointF m_pos;
    Core::SelectableTool m_tool= Core::SelectableTool::HANDLER;
    Core::Layer m_layer= Core::Layer::NONE;
    QString m_uuid;

    bool m_initialized= false;
    bool m_posEditing= false;
    bool m_rotationEditing= false;
};
} // namespace vmap
#endif // VISUALITEMCONTROLLER_H
