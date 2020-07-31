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

#include "data/mediacontainer.h"
#include "rgraphicsview.h"
#include "vmap.h"
#include "vmap/vmaptoolbar.h"
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
    VMapFrame(VectorialMapController* ctrl, QWidget* parent= nullptr);
    virtual ~VMapFrame() override;
    bool defineMenu(QMenu* /*menu*/);
    bool openFile(const QString& file);
    bool createMap();
    virtual bool readFileFromUri();
    void saveMedia() override;
    void putDataIntoCleverUri() override;
public slots:
    virtual void currentCursorChanged(QCursor*) override;
    virtual void currentToolChanged(Core::SelectableTool) override;
signals:
    void defineCurrentTool(Core::SelectableTool tool);

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

protected slots:
    virtual void updateTitle() override;

private: // functions
    void setupUi();

private:
    QPointer<VectorialMapController> m_ctrl;
    std::unique_ptr<VMap> m_vmap;
    std::unique_ptr<RGraphicsView> m_graphicView;
    std::unique_ptr<VToolsBar> m_toolbox;
    std::unique_ptr<VmapToolBar> m_toolbar;
};
#endif
