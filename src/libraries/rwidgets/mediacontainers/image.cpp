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

#include <QDebug>
#include <QInputDialog>
#include <QMenu>
#include <QScrollBar>
#include <QShortcut>

#include "controller/view_controller/imagecontroller.h"
#include "image.h"
#include "network/clientconnection.h"
#include "network/networkmessagewriter.h"
// #include "widgets/onlinepicturedialog.h"

/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/
Image::Image(ImageController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::ImageContainer, parent)
    , m_ctrl(ctrl)
    , m_imageLabel(new QLabel())
    , m_widgetArea(new QScrollArea())
    , m_NormalSize(0, 0)
{
    setObjectName("Image");
    connect(m_ctrl, &ImageController::zoomLevelChanged, this, &Image::resizeLabel);
    connect(m_ctrl, &ImageController::fitWindowChanged, this, &Image::fitWindow);
    connect(m_ctrl, &ImageController::pixmapChanged, this, &Image::initImage);
    connect(m_ctrl, &ImageController::cursorChanged, this, [this]() { m_imageLabel->setCursor(m_ctrl->cursor()); });

    setWindowIcon(QIcon::fromTheme("image-x-generic"));
    createActions();

    if(m_ctrl && m_ctrl->isMovie())
        addAction(m_playAct);

    connect(m_ctrl, &ImageController::nameChanged, this,
            [this]() { setWindowTitle(tr("%1 - Image").arg(m_ctrl->name())); });

    m_widgetArea->setAlignment(Qt::AlignCenter);
    m_imageLabel->setLineWidth(0);
    m_imageLabel->setFrameStyle(QFrame::NoFrame);
    m_widgetArea->setWidget(m_imageLabel);
    m_fitWindowAct->setChecked(m_ctrl->fitWindow());
    setWidget(m_widgetArea.get());
    initImage();
    if(m_ctrl)
        setWindowTitle(tr("%1 - Image").arg(m_ctrl->name()));
}

Image::~Image()= default;

void Image::initImage()
{
    if(!m_ctrl || m_ctrl->pixmap().isNull())
        return;

    auto pixmap= m_ctrl->pixmap();

    m_imageLabel->setPixmap(m_ctrl->scaledPixmap());
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);
    m_imageLabel->resize(pixmap.size());
    fitWorkSpace();
    fitWindow();
}

void Image::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_allowedMove= true;
        m_startingPoint= event->pos();
        m_horizontalStart= m_widgetArea->horizontalScrollBar()->value();
        m_verticalStart= m_widgetArea->verticalScrollBar()->value();
    }
    QMdiSubWindow::mousePressEvent(event);
}

void Image::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_allowedMove= false;
    }

    QMdiSubWindow::mouseReleaseEvent(event);
}

void Image::mouseMoveEvent(QMouseEvent* event)
{
    if(m_allowedMove)
    {
        QPoint diff= m_startingPoint - event->pos();
        m_widgetArea->horizontalScrollBar()->setValue(m_horizontalStart + diff.x());
        m_widgetArea->verticalScrollBar()->setValue(m_verticalStart + diff.y());
    }
    QMdiSubWindow::mouseMoveEvent(event);
}

void Image::resizeLabel()
{
    if(m_ctrl->fitWindow())
    {
        int w= m_widgetArea->viewport()->rect().width();
        int h= m_widgetArea->viewport()->rect().height();
        if(w > h * m_ctrl->ratioV())
        {
            m_imageLabel->resize(static_cast<int>(h * m_ctrl->ratioV()), h);
        }
        else
        {
            m_imageLabel->resize(w, static_cast<int>(w * m_ctrl->ratioH()));
        }
    }
    else if((m_NormalSize.height() != 0) && (m_NormalSize.width() != 0))
    {
        m_imageLabel->resize(m_ctrl->zoomLevel() * m_NormalSize);
    }
    else
    {
        m_imageLabel->resize(m_ctrl->zoomLevel() * m_ctrl->pixmap().size());
        m_NormalSize= widget()->size();
    }
}

void Image::fitWorkSpace()
{
    /* Better computation
     * if(nullptr!=parentWidget())
    {
        QSize windowsize = parentWidget()->size();//right size
        while((windowsize.height()<(m_zoomLevel * m_pixMap.height()))||(windowsize.width()<(m_zoomLevel *
    m_pixMap.width())))
        {
            m_zoomLevel -= 0.05;
        }
        setGeometry(0,0,m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel);
        m_imageLabel->resize(m_pixMap.size());
        m_NormalSize = QSize(0,0);
        resizeLabel();
        m_zoomLevel = 1.0;
    }*/

    if(nullptr == parentWidget() || m_ctrl->isMovie())
        return;

    auto pixmap= m_ctrl->pixmap();

    QSize windowsize= parentWidget()->size(); // right size
    while((windowsize.height() < (m_ctrl->zoomLevel() * pixmap.height()))
          || (windowsize.width() < (m_ctrl->zoomLevel() * pixmap.width())))
    {
        m_ctrl->zoomOut(0.1);
    }
    m_imageLabel->resize(pixmap.size());
    m_NormalSize= QSize(0, 0);
    resizeLabel();
    setGeometry(m_imageLabel->rect().adjusted(0, 0, 4, 4));
    m_ctrl->setZoomLevel(1.0);
}
void Image::wheelEvent(QWheelEvent* event)
{
    if(event->modifiers() == Qt::ControlModifier)
    {
        int delta= event->angleDelta().y();
        (delta > 0) ? m_ctrl->zoomIn() : m_ctrl->zoomOut();
        event->ignore();
    }
    QMdiSubWindow::wheelEvent(event);
}

void Image::paintEvent(QPaintEvent* event)
{
    QMdiSubWindow::paintEvent(event);
    if(m_ctrl->fitWindow())
    {
        resizeLabel();
    }
    else if((m_NormalSize * m_ctrl->zoomLevel()) != m_imageLabel->size())
    {
        m_NormalSize= m_imageLabel->size() / m_ctrl->zoomLevel();
        m_windowSize= size();
    }
}

void Image::updateTitle()
{
    setWindowTitle(
        tr("%1 - (Picture : %2)").arg(m_ctrl->name()).arg(m_ctrl->sharing() ? tr("Shared") : tr("Unshared")));
}

void Image::createActions()
{
    m_actionZoomIn= new QAction(tr("Zoom In"), this);
    m_actionZoomIn->setToolTip(tr("increase zoom level"));
    m_actionZoomIn->setIcon(QIcon::fromTheme("zoom-in-32"));
    connect(m_actionZoomIn, &QAction::triggered, this, [this] { m_ctrl->zoomIn(); });

    m_zoomInShort= new QShortcut(QKeySequence(tr("Ctrl++", "Zoom In")), this);
    m_zoomInShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_zoomInShort, &QShortcut::activated, this, [this] { m_ctrl->zoomIn(); });
    m_actionZoomIn->setShortcut(m_zoomInShort->key());

    m_actionZoomOut= new QAction(tr("Zoom out"), this);
    m_actionZoomOut->setIcon(QIcon::fromTheme("zoom-out-32"));
    m_actionZoomOut->setToolTip(tr("Reduce zoom level"));
    connect(m_actionZoomOut, &QAction::triggered, this, [this] { m_ctrl->zoomOut(); });

    m_zoomOutShort= new QShortcut(QKeySequence(tr("Ctrl+-", "Zoom Out")), this);
    m_zoomOutShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_zoomOutShort, &QShortcut::activated, this, [this] { m_ctrl->zoomOut(); });
    m_actionZoomOut->setShortcut(m_zoomOutShort->key());

    auto fitWorkspace= [this]
    {
        m_imageLabel->resize(parentWidget()->size());
        fitWorkSpace();
    };

    m_actionfitWorkspace= new QAction(tr("Fit the workspace"), this);
    m_actionfitWorkspace->setIcon(QIcon::fromTheme("fit-page"));
    m_actionfitWorkspace->setToolTip(tr("The window and the image fit the workspace"));
    connect(m_actionfitWorkspace, &QAction::triggered, this, fitWorkspace);

    m_fitShort= new QShortcut(QKeySequence(tr("Ctrl+m", "Fit the workspace")), this);
    m_fitShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_fitShort, &QShortcut::activated, this, fitWorkspace);
    m_actionfitWorkspace->setShortcut(m_fitShort->key());

    auto setFitWindow= [this] { m_ctrl->setFitWindow(!m_ctrl->fitWindow()); };

    m_fitWindowAct= new QAction(tr("Fit Window"), this);
    m_fitWindowAct->setCheckable(true);
    connect(m_ctrl, &ImageController::fitWindowChanged, this,
            [this] { m_fitWindowAct->setChecked(m_ctrl->fitWindow()); });
    m_fitWindowAct->setIcon(QIcon::fromTheme("fit-window"));
    m_fitWindowAct->setToolTip(tr("Image will take the best dimension to fit the window."));
    connect(m_fitWindowAct, &QAction::triggered, this, setFitWindow);

    m_fitWindowShort= new QShortcut(QKeySequence(tr("Ctrl+f", "Fit the window")), this);
    m_fitWindowShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_fitWindowShort, &QShortcut::activated, this, setFitWindow);
    m_fitWindowAct->setShortcut(m_fitWindowShort->key());

    auto zoomLittle= [this] { m_ctrl->setZoomLevel(0.2); };

    m_actionlittleZoom= new QAction(tr("Little"), this);
    m_actionlittleZoom->setToolTip(tr("Set the zoom level at 20% "));
    connect(m_actionlittleZoom, &QAction::triggered, this, zoomLittle);

    m_littleShort= new QShortcut(QKeySequence(tr("Ctrl+l", "Set the zoom level at 20%")), this);
    m_littleShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_littleShort, &QShortcut::activated, this, zoomLittle);
    m_actionlittleZoom->setShortcut(m_littleShort->key());

    auto zoomNormal= [this] { m_ctrl->setZoomLevel(1.0); };

    m_actionNormalZoom= new QAction(tr("Normal"), this);
    m_actionNormalZoom->setToolTip(tr("No Zoom"));
    connect(m_actionNormalZoom, &QAction::triggered, this, zoomNormal);

    m_normalShort= new QShortcut(QKeySequence(tr("Ctrl+n", "Normal")), this);
    m_normalShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_normalShort, &QShortcut::activated, this, zoomNormal);
    m_actionNormalZoom->setShortcut(m_normalShort->key());

    auto zoomBig= [this] { m_ctrl->setZoomLevel(4.0); };

    m_actionBigZoom= new QAction(tr("Big"), this);
    m_actionBigZoom->setToolTip(tr("Set the zoom level at 400%"));
    connect(m_actionBigZoom, &QAction::triggered, this, zoomBig);

    m_bigShort= new QShortcut(QKeySequence(tr("Ctrl+b", "Zoom Out")), this);
    m_bigShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_bigShort, &QShortcut::activated, this, zoomBig);
    m_actionBigZoom->setShortcut(m_bigShort->key());

    m_rename= new QAction(tr("Rename"), this);
    m_rename->setShortcut(Qt::Key_F2);
    connect(m_rename, &QAction::triggered, this,
            [this]()
            {
                auto text= QInputDialog::getText(this, tr("Define the new name"), tr("New name:"), QLineEdit::Normal,
                                                 m_ctrl->name());
                if(text.isEmpty())
                    return;
                m_ctrl->setName(text);
            });

    m_playAct= new QAction(tr("Play"), this);
    m_playAct->setShortcut(Qt::Key_Space);
    m_stopAct= new QAction(tr("Stop"), this);

    connect(m_ctrl, &ImageController::statusChanged, this,
            [this](ImageController::Status status)
            {
                switch(status)
                {
                case ImageController::Playing:
                    m_playAct->setText(tr("Pause"));
                    m_stopAct->setEnabled(true);
                    break;
                case ImageController::Paused:
                    m_playAct->setText(tr("Play"));
                    m_stopAct->setEnabled(true);
                    break;
                case ImageController::Stopped:
                    m_stopAct->setEnabled(false);
                    m_playAct->setText(tr("Play"));
                    break;
                }
            });

    m_share.reset(new QAction(tr("Share")));
    m_share->setCheckable(true);
    m_share->setChecked(true);

    connect(m_playAct, &QAction::triggered, m_ctrl, &ImageController::play);
    connect(m_stopAct, &QAction::triggered, m_ctrl, &ImageController::stop);
    connect(m_share.get(), &QAction::triggered, m_ctrl, &ImageController::setSharing);

    connect(m_ctrl, &ImageController::sharingChanged, m_share.get(),
            [this]()
            {
                if(m_ctrl->sharing())
                    m_share->setText(tr("Stop Sharing"));
                else
                    m_share->setText(tr("Share"));

                updateTitle();
            });
}
void Image::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    if(m_ctrl->isMovie())
    {
        menu.addAction(m_playAct);
        menu.addAction(m_stopAct);
        menu.addSeparator();
    }

    menu.addAction(m_rename);
    menu.addAction(m_share.get());
    menu.addSeparator();
    menu.addAction(m_actionZoomIn);
    menu.addAction(m_actionZoomOut);
    menu.addSeparator();
    menu.addAction(m_actionfitWorkspace);
    menu.addAction(m_fitWindowAct);
    addActionToMenu(menu);
    menu.addSeparator();
    menu.addAction(m_actionlittleZoom);
    menu.addAction(m_actionNormalZoom);
    menu.addAction(m_actionBigZoom);

    menu.exec(event->globalPos() /*event->pos()*/);
}
void Image::fitWindow()
{
    auto fit= m_fitWindowAct->isChecked();
    resizeLabel();
    if(fit)
    {
        m_widgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_widgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else
    {
        m_widgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_widgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    m_actionZoomIn->setEnabled(!fit);
    m_actionZoomOut->setEnabled(!fit);
    m_actionlittleZoom->setEnabled(!fit);
    m_actionNormalZoom->setEnabled(!fit);
    m_actionBigZoom->setEnabled(!fit);

    update();
}
void Image::resizeEvent(QResizeEvent* event)
{
    if(m_fitWindowAct->isChecked())
    {
        resizeLabel();
    }

    QMdiSubWindow::resizeEvent(event);
}

void Image::setParent(QWidget* parent)
{
    QMdiSubWindow::setParent(parent);
    fitWorkSpace();
}
