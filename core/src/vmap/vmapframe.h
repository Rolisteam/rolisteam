/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
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

#ifndef MAP_FRAME_H
#define MAP_FRAME_H

#include <QAction>
#include <QImage>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QPointer>
#include <QWidget>
#include <memory>

#include "rgraphicsview.h"

#include "data/mediacontainer.h"
#include "network/networkreceiver.h"
#include "vmap.h"
#include "vmap/vtoolbar.h"

class VectorialMapController;

/**
 * @brief displays, stores and manages a map and its items
 *
 */
class VMapFrame : public MediaContainer
{
    Q_OBJECT

public:
    /**
     * @brief The PermissionMode enum
     */
    enum PermissionMode
    {
        GM_ONLY,
        PC_MOVE,
        PC_ALL
    };
    /**
     * @brief VMapFrame
     */
    VMapFrame(VectorialMapController* ctrl, QWidget* parent= nullptr);
    /**
     * @brief destructor
     */
    virtual ~VMapFrame() override;
    /**
     * @brief fills up the current window menu
     */
    bool defineMenu(QMenu* /*menu*/);

    /**
     * @brief reads the map into the given file
     * @param uri of the file
     */
    bool openFile(const QString& file);
    /**
     * @brief createMap
     * @return
     */
    bool createMap();
    /**
     * @brief readFile
     * @return
     */
    virtual bool readFileFromUri() override;
    /**
     * @brief processAddItemMessage should add items from Network
     * @param msg
     */
    void processAddItemMessage(NetworkMessageReader* msg);
    /**
     * @brief processDelItemMessage deletes items from network order
     * @param msg
     */
    void processDelItemMessage(NetworkMessageReader* msg);
    /**
     * @brief processMoveItemMessage
     * @param msg
     */
    void processMoveItemMessage(NetworkMessageReader* msg);
    /**
     * @brief processGeometryChangeItem
     * @param msg
     */
    void processGeometryChangeItem(NetworkMessageReader* msg);
    /**
     * @brief processMapPropertyChange
     * @param msg
     */
    void processMapPropertyChange(NetworkMessageReader* msg);
    /**
     * @brief processSetParentItem
     * @param msg
     */
    void processSetParentItem(NetworkMessageReader* msg);
    /**
     * @brief saveMedia
     */
    void saveMedia(const QString&) override;
    /**
     * @brief processGeometryViewChange
     * @param msg
     */
    void processGeometryViewChange(NetworkMessageReader* msg);
    /**
     * @brief processOpacityMessage
     * @param msg
     */
    void processOpacityMessage(NetworkMessageReader* msg);

    void fill(NetworkMessageWriter& msg) override;
    void readMessage(NetworkMessageReader& msg) override;

    void processsZValueMsg(NetworkMessageReader* msg);
    void processsRotationMsg(NetworkMessageReader* msg);
    void processsRectGeometryMsg(NetworkMessageReader* msg);

    void putDataIntoCleverUri() override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);
public slots:
    /**
     *  @brief change the current mice cursor
     *  @param new selected QCursor
     */
    virtual void currentCursorChanged(QCursor*) override;
    /**
     *  @brief change the current drawing tool
     *  @param  new selected tool
     */
    virtual void currentToolChanged(Core::SelectableTool) override;

    /**
     *  @brief change the current editing  behavior to MaskMode.
     */
    // virtual void setEditingMode(int);

    // void processLayerMessage(NetworkMessageReader* msg);
signals:
    /**
     * @brief defineCurrentTool
     * @param tool
     */
    void defineCurrentTool(Core::SelectableTool tool);

protected:
    /**
     *  @brief called when painting the widget is required
     *  @param event discribe the context of the event
     */
    // virtual void paintEvent(QPaintEvent* event);
    virtual void keyPressEvent(QKeyEvent* event) override;

protected slots:
    /**
     * @brief updateTitle
     */
    virtual void updateTitle() override;

private slots:
    void setupUi();

private: // functions
    /**
     * @brief updateMap
     */
    void updateMap();
    /**
     * @brief visibilityModeToText
     * @return
     */
    QString visibilityModeToText(Core::VisibilityMode);
    /**
     * @brief permissionModeToText
     * @return
     */
    QString permissionModeToText(Core::PermissionMode);
    /**
     * @brief layerToText
     * @return
     */
    QString layerToText(Core::Layer);

private:
    QPointer<VectorialMapController> m_ctrl;
    // VMap* m_vmap= nullptr;
    std::unique_ptr<RGraphicsView> m_graphicView;
};

#endif
