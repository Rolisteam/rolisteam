/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef CHARACTERITEM_H
#define CHARACTERITEM_H

#include <QAction>
#include <QPointer>

#include "data/characterstate.h"
#include "data/charactervision.h"
#include "data/person.h"
#include "diceparser/diceparser.h"
#include "rwidgets_global.h"
#include "visualitem.h"
#include <memory>
namespace vmap
{
class CharacterItemController;
}
/**
 * @brief represents any character on map.
 */
class RWIDGET_EXPORT CharacterItem : public VisualItem
{
    Q_OBJECT
public:
    enum Corner
    {
        TopLeft,
        TopRight,
        BottomRight,
        BottomLeft,
        SightAngle,
        SightLenght
    };
    Q_ENUM(Corner)
    /**
     * @brief CharacterItem
     */
    CharacterItem(vmap::CharacterItemController* ctrl);
    ~CharacterItem() override;

    // Override
    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void setNewEnd(const QPointF& nend) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
    void setGeometryPoint(qreal pointId, QPointF& pos) override;
    virtual void initChildPointItem() override;
    void resizeContents(const QRectF& rect, int pointId, TransformType transformType= KeepRatio) override;
    void updateChildPosition() override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    virtual void addActionContextMenu(QMenu&) override;
    virtual VisualItem* getItemCopy() override;
    virtual bool isLocal() const override;
    virtual void setSize(QSizeF size) override;
    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h) override;
    void setChildrenVisible(bool b) override;

    // accessors
    QString getCharacterId() const;
    bool isNpc() const;
    QString getParentId() const;
    const QPointF& getCenter() const;
    int getChildPointCount() const;
    ChildPointItem* getRadiusChildWidget() const;
    bool isPlayableCharacter() const;
    Character* getCharacter() const;

    void updateItemFlags() override;
    void addChildPoint(ChildPointItem* item);
    void readCharacterStateChanged(NetworkMessageReader& msg);
    void readVisionMsg(NetworkMessageReader* msg);
    void sendVisionMsg();
    void updateCharacter();
    void readCharacterChanged(NetworkMessageReader& msg);
    void setCharacter(Character* character);
    void setTokenFile(QString);
    void setNumber(int);
    QString getName() const;
    int getNumber() const;
    const QPainterPath getTokenShape() const;
signals:
    void positionChanged();
    void geometryChangeOnUnkownChild(qreal pointId, QPointF& F);
    void localItemZValueChange(CharacterItem*);
    void ownerChanged(Character* old, CharacterItem*);
    void runDiceCommand(QString cmd, QString uuid);

public slots:
    void changeVisionShape();
    void sizeChanged(qreal m_size);
    void endOfGeometryChange(ChildPointItem::Change change) override;
    void generatedThumbnail();
    void cleanInit();
    void runInit();

protected:
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event) override;
protected slots:
    void runCommand();
    void setShape();
private slots:
    void createActions() override;
    void characterStateChange();
    void changeCharacter();

private:
    void visionChanged();
    void initChildPointItemMotion();

private:
    QPointer<vmap::CharacterItemController> m_itemCtrl;

    // QAction
    QAction* m_visionShapeDisk= nullptr;
    QAction* m_visionShapeAngle= nullptr;
    QAction* m_reduceLife= nullptr;
    QAction* m_increaseLife= nullptr;

    bool m_protectGeometryChange;
    bool m_visionChanged;
};

#endif // CHARACTERITEM_H
