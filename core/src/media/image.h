/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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

#ifndef IMAGE_H
#define IMAGE_H

#include <QAction>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QPointer>
#include <QScrollArea>
#include <QString>
#include <QWidget>
#include <memory>

#include "preferences/preferencesmanager.h"

#include "data/mediacontainer.h"

class NetworkLink;
class NetworkMessageWriter;
class QShortcut;
class ImageController;
/**
 * @brief The Image class displays image to the screen. It also manages image from Internet and the zooming.
 */
class Image : public MediaContainer
{
    Q_OBJECT
public:
    explicit Image(ImageController* ctrl, QWidget* parent= nullptr);
    /**
     * @brief ~Image destructor.
     */
    virtual ~Image();
    /**
     * @brief setInternalAction may not be useful anymore.
     * @param action
     */
    void setInternalAction(QAction* action);
    QAction* getAssociatedAction() const;

    //    void saveImageToFile(QFile& file);
    // void saveImageToFile(QDataStream& out);
    bool isImageOwner(QString id);
    void setParent(QWidget* parent);
    void setImage(QImage& img);

    virtual bool readFileFromUri();
    virtual bool openMedia();
    virtual void saveMedia(const QString&);

    virtual void putDataIntoCleverUri();

    /*virtual void fill(NetworkMessageWriter& msg);
    virtual void readMessage(NetworkMessageReader& msg);*/

protected:
    /**
     * @brief called when users roll the wheel of their mouse
     * @param event : define few parameters about the action (way,..)
     */
    void wheelEvent(QWheelEvent* event);

    /**
     * @brief resizeEvent make sure the window keep the right ratio.
     */
    void resizeEvent(QResizeEvent* event);

    // void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void paintEvent(QPaintEvent* event);

protected slots:
    virtual void updateTitle();
private slots:
    /**
     * @brief refresh the size of the label (which embeds the picture)
     */
    void resizeLabel();
    /**
     * @brief fitWindow set the size of the picture at the best size of the window.
     */
    void fitWindow();

private:
    /**
     * @brief adapt the size window to fit the MdiArea size and no scrollbar (if possible)
     */
    void fitWorkSpace();
    void createActions();
    void initImage();

private:
    QPointer<ImageController> m_ctrl;
    QLabel* m_imageLabel;
    std::unique_ptr<QScrollArea> m_widgetArea;
    QPoint m_startingPoint;
    int m_horizontalStart;
    int m_verticalStart;
    bool m_allowedMove;
    QSize m_NormalSize;
    QSize m_windowSize;

    QAction* m_actionZoomIn;
    QShortcut* m_zoomInShort;

    QAction* m_actionZoomOut;
    QShortcut* m_zoomOutShort;

    QAction* m_actionfitWorkspace;
    QShortcut* m_fitShort;

    QAction* m_actionNormalZoom; // *1
    QShortcut* m_normalShort;

    QAction* m_actionBigZoom; // * 4
    QShortcut* m_bigShort;

    QAction* m_actionlittleZoom; // * 0.2
    QShortcut* m_littleShort;

    // fit window keeping ratio
    QAction* m_fitWindowAct; // * 0.2
    QAction* m_playAct;
    QAction* m_stopAct;

    QShortcut* m_fitWindowShort;

    double m_ratioImage;
    double m_ratioImageBis;
};

#endif
