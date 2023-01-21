/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef MINDMAPCONTROLLERBASE_H
#define MINDMAPCONTROLLERBASE_H

#include <QAbstractItemModel>
#include <QMimeData>
#include <QObject>
#include <QPointer>
#include <QRectF>
#include <QUndoStack>
#include <memory>

#include "controller/view_controller/mediacontrollerbase.h"

#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/controller/spacingcontroller.h"
#include "mindmap/data/positioneditem.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/minditemmodel.h"
#include <core_global.h>

namespace mindmap
{
class NodeStyle;
class MindNode;
class NodeStyleModel;
class MindItemModel;
class MindItem;
class LinkController;

class CORE_EXPORT MindMapControllerBase : public MediaControllerBase
{
    Q_OBJECT
    Q_PROPERTY(MindItemModel* itemModel READ itemModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* styleModel READ styleModel CONSTANT)
    Q_PROPERTY(int defaultStyleIndex READ defaultStyleIndex WRITE setDefaultStyleIndex NOTIFY defaultStyleIndexChanged)
    Q_PROPERTY(bool spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(SelectionController* selectionCtrl READ selectionController CONSTANT)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(QString errorMsg READ errorMsg WRITE setErrorMsg NOTIFY errorMsgChanged)
    Q_PROPERTY(QRectF contentRect READ contentRect NOTIFY contentRectChanged)
    Q_PROPERTY(bool linkLabelVisibility READ linkLabelVisibility WRITE setLinkLabelVisibility NOTIFY
                   linkLabelVisibilityChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
    Q_PROPERTY(mindmap::ImageModel* imgModel READ imgModel CONSTANT)
    Q_PROPERTY(mindmap::SpacingController* spacingCtrl READ spacingCtrl CONSTANT)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(bool hasNetwork READ hasNetwork CONSTANT)
public:
    explicit MindMapControllerBase(bool hasNetwork, const QString& id, QObject* parent= nullptr);
    virtual ~MindMapControllerBase();

    mindmap::MindItemModel* itemModel() const;
    QAbstractItemModel* styleModel() const;
    mindmap::ImageModel* imgModel() const;
    mindmap::SpacingController* spacingCtrl() const;

    mindmap::SelectionController* selectionController() const;
    const QString& errorMsg() const;
    QRectF contentRect() const;

    bool spacing() const;
    bool canRedo() const;
    bool canUndo() const;
    int defaultStyleIndex() const;
    Q_INVOKABLE mindmap::NodeStyle* style(int index) const;

    bool linkLabelVisibility() const;
    void setLinkLabelVisibility(bool newLinkLabelVisibility);

    bool hasSelection() const;

    mindmap::LinkController* linkFromId(const QString& id) const;
    mindmap::MindNode* nodeFromId(const QString& id) const;
    void createLink(const QString& id, const QString& id2);
    void addLink(const QList<mindmap::LinkController*>& link, bool network= false);
    void addNode(const QList<MindNode*>& nodes, bool network= false);
    bool pasteData(const QMimeData& mimeData);
    void setCurrentPackage(mindmap::PositionedItem* item);

    QObject* subItem(const QString& id, mindmap::MindItem::Type type) const;

    qreal zoomLevel() const;
    void setZoomLevel(qreal newZoomLevel);

    bool hasNetwork() const;

signals:
    void spacingChanged();
    void canRedoChanged();
    void canUndoChanged();
    void errorMsgChanged();
    void defaultStyleIndexChanged();
    void contentRectChanged();
    void linkLabelVisibilityChanged();
    void hasSelectionChanged();

    void zoomLevelChanged();

public slots:
    void resetData();
    void setSpacing(bool b);
    void redo();
    void undo();
    void setErrorMsg(const QString& msg);
    void importFile(const QString& path);
    void setDefaultStyleIndex(int indx);

    void addNode(const QString& idparent);
    void addPackage(const QPointF& pos);
    void updatePackage(const QPointF& pos);
    void addLink(const QString& start, const QString& id);

    void reparenting(mindmap::MindItem* parent, const QString& id);
    void removeSelection();

    void centerItems(qreal w, qreal h);
    void addImageFor(const QString& idNode, const QString& path);
    void addItemIntoPackage(const QString& idNode, const QString& idPack);
    void refresh();
    void openImage(const QString& id, const QUrl& path);
    void removeImage(const QString& id);
    void removeLink(const QStringList& id);
    void removeNode(const QStringList& id);

protected:
    void clearData();
    void setContentRect(const QRectF& rect);

protected:
    std::unique_ptr<SpacingController> m_spacingController;
    std::unique_ptr<SelectionController> m_selectionController;
    std::unique_ptr<ImageModel> m_imgModel;
    std::unique_ptr<MindItemModel> m_itemModel;
    std::unique_ptr<NodeStyleModel> m_styleModel;
    std::unique_ptr<QThread> m_spacing;
    QUndoStack m_stack;
    int m_defaultStyleIndex= 0;
    QPointer<PositionedItem> m_package;
    QString m_errorMsg;
    bool m_linkLabelVisibility;
    QRectF m_contentRect;
    qreal m_zoomLevel= 1.0;
    bool m_hasNetwork;
};
} // namespace mindmap
#endif // MINDMAPCONTROLLERBASE_H
