/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
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
#include "workspace.h"

#include <QtGui>

#include "controller/contentcontroller.h"
/*#include "controller/media_controller/charactersheetmediacontroller.h"
#include "controller/media_controller/imagemediacontroller.h"
#include "controller/media_controller/notemediacontroller.h"
#include "controller/media_controller/sharednotemediacontroller.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "controller/media_controller/webpagemediacontroller.h"*/

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mapcontroller.h"
#include "controller/view_controller/notecontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"

#ifdef WITH_PDF
#include "controller/media_controller/pdfmediacontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#endif

#include "controller/instantmessagingcontroller.h"
#include "qmlchat/instantmessagingview.h"

#include "media/charactersheetwindow.h"
#include "media/image.h"
#include "noteeditor/src/notecontainer.h"
#include "pdfviewer/pdfviewer.h"
#include "sharededitor/sharednotecontainer.h"
#include "vmap/vmapframe.h"
#include "webview/webview.h"

#define GRAY_SCALE 191

Workspace::Workspace(ContentController* ctrl, InstantMessagingController* instantCtrl, QWidget* parent)
    : QMdiArea(parent)
    , m_ctrl(ctrl)
    , m_variableSizeBackground(size())
    , m_actionSubWindowMap(new QMap<QAction*, QMdiSubWindow*>())
{

    connect(m_ctrl, &ContentController::maxLengthTabNameChanged, this, &Workspace::updateTitleTab);
    connect(m_ctrl, &ContentController::shortTitleTabChanged, this, &Workspace::updateTitleTab);
    connect(m_ctrl, &ContentController::workspaceFilenameChanged, this, [this]() {
        m_backgroundPicture= QPixmap(m_ctrl->workspaceFilename());
        updateBackGround();
    });
    connect(m_ctrl, &ContentController::workspaceColorChanged, this, &Workspace::updateBackGround);
    connect(m_ctrl, &ContentController::workspacePositioningChanged, this, &Workspace::updateBackGround);

    connect(m_ctrl, &ContentController::mediaControllerCreated, this, &Workspace::addMedia);

    /* connect(m_ctrl->imagesCtrl(), &ImageMediaController::imageControllerCreated, this, &Workspace::addImage);
       connect(m_ctrl->noteCtrl(), &NoteMediaController::noteControllerCreated, this, &Workspace::addNote);
       connect(m_ctrl->vmapCtrl(), &VectorialMapMediaController::vmapControllerCreated, this,
   &Workspace::addVectorialMap); connect(m_ctrl->webPageCtrl(), &WebpageMediaController::webpageControllerCreated, this,
   &Workspace::addWebpage); #ifdef WITH_PDF connect(m_ctrl->pdfCtrl(), &PdfMediaController::pdfControllerCreated, this,
   &Workspace::addPdf); #endif connect(m_ctrl->sheetCtrl(), &CharacterSheetMediaController::characterSheetCreated, this,
               &Workspace::addCharacterSheet);
       connect(m_ctrl->sharedCtrl(), &SharedNoteMediaController::sharedNoteControllerCreated, this,
               &Workspace::addSharedNote);*/

    connect(this, &Workspace::subWindowActivated, this, &Workspace::updateActiveMediaContainer);

    m_instantMessageView= addSubWindow(new InstantMessagingView(instantCtrl));
    m_instantMessageView->setGeometry(0, 0, 400, 600);
    m_instantMessageView->setWindowIcon(QIcon::fromTheme("chatIcon"));
    // m_instantMessageView->setVisible(false);

    auto imvAction= new QAction(tr("Instant Messging"), this);
    insertActionAndSubWindow(imvAction, m_instantMessageView);
    connect(imvAction, &QAction::triggered, m_instantMessageView, &QDockWidget::setVisible);
    connect(imvAction, &QAction::triggered, m_instantMessageView->widget(), &InstantMessagingView::setVisible);

    m_backgroundPicture= QPixmap(m_ctrl->workspaceFilename());
    updateBackGround();
}

Workspace::~Workspace()
{
    if(nullptr != m_actionSubWindowMap)
    {
        delete m_actionSubWindowMap;
        m_actionSubWindowMap= nullptr;
    }
}

void Workspace::updateBackGround()
{
    QBrush background;
    background.setColor(m_ctrl->workspaceColor());
    setBackground(background);

    m_variableSizeBackground= m_variableSizeBackground.scaled(size());
    m_variableSizeBackground.fill(m_ctrl->workspaceColor());
    QPainter painter(&m_variableSizeBackground);

    int x= 0;
    int y= 0;
    int w= m_backgroundPicture.width();
    int h= m_backgroundPicture.height();
    bool repeated= false;
    switch(m_ctrl->workspacePositioning())
    {
    case TopLeft:
        break;
    case BottomLeft:
        y= m_variableSizeBackground.size().height() - h;
        break;
    case Center:
        x= m_variableSizeBackground.width() / 2 - w / 2;
        y= m_variableSizeBackground.height() / 2 - h / 2;
        break;
    case TopRight:
        x= m_variableSizeBackground.width() - w;
        break;
    case BottomRight:
        x= m_variableSizeBackground.width() - w;
        y= m_variableSizeBackground.height() - h;
        break;
    case Scaled:
    {
        qreal rd= static_cast<qreal>(w) / static_cast<qreal>(h);
        if(rd > 1)
        {
            w= m_variableSizeBackground.width();
            h= static_cast<int>(w / rd);
        }
        else
        {
            h= m_variableSizeBackground.height();
            w= static_cast<int>(h / rd);
        }
        x= m_variableSizeBackground.width() / 2 - w / 2;
        y= m_variableSizeBackground.height() / 2 - h / 2;
    }
    break;
    case Filled:
        w= m_variableSizeBackground.width();
        h= m_variableSizeBackground.height();
        break;
    case Repeated:
        repeated= true;
        break;
    }
    if(!repeated)
    {
        painter.drawPixmap(x, y, w, h, m_backgroundPicture);
        setBackground(QBrush(m_variableSizeBackground));
    }
    else
    {
        setBackground(QBrush(m_backgroundPicture));
    }
    update();
}

void Workspace::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    if((m_variableSizeBackground.size() == this->size()))
    {
        return;
    }

    updateBackGround();

    QMdiArea::resizeEvent(event);
}
QWidget* Workspace::addWindow(QWidget* child, QAction* action)
{
    QMdiSubWindow* sub= addSubWindow(child);
    if(viewMode() == QMdiArea::TabbedView)
    {
        action->setChecked(true);
        sub->setVisible(true);
        child->setVisible(true);
    }
    insertActionAndSubWindow(action, sub);
    connect(action, &QAction::triggered, this, &Workspace::ensurePresent);
    sub->setAttribute(Qt::WA_DeleteOnClose, false);
    child->setAttribute(Qt::WA_DeleteOnClose, false);
    sub->setObjectName(child->objectName());

    sub->installEventFilter(this);
    return sub;
}

void Workspace::addContainerMedia(MediaContainer* mediac)
{
    if(nullptr == mediac)
        return;

    addSubWindow(mediac);
    insertActionAndSubWindow(mediac->getAction(), mediac);
    if(viewMode() == QMdiArea::TabbedView)
    {
        mediac->setVisible(true);
    }
    mediac->setAttribute(Qt::WA_DeleteOnClose, false);
    if(nullptr != mediac->widget())
    {
        mediac->widget()->setAttribute(Qt::WA_DeleteOnClose, false);
    }
    mediac->installEventFilter(this);
}
void Workspace::removeMediaContainer(MediaContainer* mediac)
{
    removeSubWindow(mediac);
    m_actionSubWindowMap->remove(mediac->getAction());
    mediac->removeEventFilter(this);
}
void Workspace::insertActionAndSubWindow(QAction* act, QMdiSubWindow* sub)
{
    m_actionSubWindowMap->insert(act, sub);
}
void Workspace::setTabbedMode(bool isTabbed)
{
    if(!isTabbed)
    {
        setViewMode(QMdiArea::SubWindowView);
        updateTitleTab();
        return;
    }

    setViewMode(QMdiArea::TabbedView);
    // setTabsClosable ( true );
    setTabsMovable(true);
    setTabPosition(QTabWidget::North);
    /// make all subwindows visible.
    auto keys= m_actionSubWindowMap->keys();
    for(QAction* act : keys)
    {
        auto tmp= m_actionSubWindowMap->value(act);
        if(nullptr == tmp)
        {
            tmp->setVisible(true);
            if(nullptr != act)
            {
                act->setChecked(true);
            }
            if(nullptr != tmp->widget())
            {
                tmp->widget()->setVisible(true);
            }
        }
    }
    updateTitleTab();
}
bool Workspace::updateTitleTab()
{
    bool shortName= m_ctrl->shortTitleTab(); // m_preferences->value("shortNameInTabMode", false).toBool();
    int textLength= m_ctrl->maxLengthTabName();
    if((viewMode() == QMdiArea::TabbedView) && (shortName))
    {
        auto values= m_actionSubWindowMap->values();
        for(auto& subwindow : values)
        {
            auto title= subwindow->windowTitle();
            if(title.size() > textLength)
            {
                m_titleBar.insert(subwindow, title);
                title= title.left(textLength);
                subwindow->setWindowTitle(title);
            }
        }
    }
    else
    {
        auto keys= m_titleBar.keys();
        for(auto& key : keys)
        {
            auto title= m_titleBar.value(key);
            key->setWindowTitle(title);
        }
        m_titleBar.clear();
    }
    return true;
}
bool Workspace::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::Close)
    {
        QMdiSubWindow* sub= dynamic_cast<QMdiSubWindow*>(object);
        if(nullptr != sub)
        {
            sub->setVisible(false);
            event->accept();
            return true;
        }
    }
    return QMdiArea::eventFilter(object, event);
}
void Workspace::ensurePresent()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(nullptr != act)
    {
        if(!subWindowList().contains(m_actionSubWindowMap->value(act)))
        {
            m_actionSubWindowMap->value(act)->widget()->setVisible(true);
            addSubWindow(m_actionSubWindowMap->value(act));
        }
    }
}

QVector<QMdiSubWindow*> Workspace::getAllSubWindowFromId(const QString& id) const
{
    QVector<QMdiSubWindow*> vector;

    for(auto& tmp : subWindowList())
    {
        if(nullptr != tmp->widget())
        {
            MediaContainer* tmpWindow= dynamic_cast<MediaContainer*>(tmp);
            if(nullptr != tmpWindow)
            {
                if(tmpWindow->mediaId() == id)
                {
                    vector.append(tmp);
                }
            }
        }
    }
    return vector;
}

/*QMdiSubWindow* Workspace::getSubWindowFromId(QString id)
{
    for(auto& tmp : subWindowList())
    {
        if(nullptr != tmp->widget())
        {
            MediaContainer* tmpWindow= dynamic_cast<MediaContainer*>(tmp->widget());
            if(nullptr != tmpWindow)
            {
                if(tmpWindow->getMediaId() == id)
                {
                    return tmp;
                }
            }
        }
    }
    return nullptr;
}*/

bool Workspace::closeAllSubs()
{
    std::for_each(m_mediaContainers.begin(), m_mediaContainers.end(),
                  [this](const std::unique_ptr<MediaContainer>& media) { closeSub(media.get()); });

    return true;
}

bool Workspace::closeActiveSub()
{
    bool status= false;

    if(m_activeMediaContainer.isNull())
        return status;

    status= closeSub(m_activeMediaContainer);

    return status;
}

bool Workspace::closeSub(MediaContainer* container)
{
    if(!container)
        return false;

    auto ctrl= m_activeMediaContainer->ctrl();

    ctrl->askToClose();
    return true;
}

void Workspace::updateActiveMediaContainer(QMdiSubWindow* window)
{
    auto activeMediaContainer= dynamic_cast<MediaContainer*>(window);

    if(m_activeMediaContainer == activeMediaContainer)
        return;

    if(!m_activeMediaContainer.isNull())
    {
        auto ctrl= m_activeMediaContainer->ctrl();
        if(ctrl)
            ctrl->setActive(false);
    }

    if(nullptr != activeMediaContainer)
    {
        auto ctrl= activeMediaContainer->ctrl();
        ctrl->setActive(true);
        if(activeMediaContainer->getContainerType() == MediaContainer::ContainerType::VMapContainer)
            emit vmapActive();
    }

    m_activeMediaContainer= activeMediaContainer;
}

void Workspace::addMedia(MediaControllerBase* ctrl)
{
    switch(ctrl->contentType())
    {
    case Core::ContentType::ONLINEPICTURE:
    case Core::ContentType::PICTURE:
        addImage(dynamic_cast<ImageController*>(ctrl));
        break;
    case Core::ContentType::WEBVIEW:
        addWebpage(dynamic_cast<WebpageController*>(ctrl));
        break;
#ifdef WITH_PDF
    case Core::ContentType::PDF:
        addPdf(dynamic_cast<PdfController*>(ctrl));
        break;
#endif
    case Core::ContentType::NOTES:
        addNote(dynamic_cast<NoteController*>(ctrl));
        break;
    case Core::ContentType::SHAREDNOTE:
        addSharedNote(dynamic_cast<SharedNoteController*>(ctrl));
        break;
    case Core::ContentType::VECTORIALMAP:
        addVectorialMap(dynamic_cast<VectorialMapController*>(ctrl));
        break;
    case Core::ContentType::CHARACTERSHEET:
        addCharacterSheet(dynamic_cast<CharacterSheetController*>(ctrl));
        break;
    default:
        break;
    }
}

void Workspace::addWidgetToMdi(QWidget* wid, QString title)
{
    wid->setParent(this);
    QMdiSubWindow* sub= addSubWindow(wid);
    // sub->setWindowTitle(title);
    // wid->setWindowTitle(title);
    sub->setVisible(true);
    wid->setVisible(true);
}
void Workspace::addImage(ImageController* ctrl)
{
    if(nullptr == ctrl)
        return;
    std::unique_ptr<MediaContainer> img(new Image(ctrl));
    addWidgetToMdi(img.get(), ctrl->name());
    m_mediaContainers.push_back(std::move(img));
}
void Workspace::addVectorialMap(VectorialMapController* ctrl)
{
    if(nullptr == ctrl)
        return;
    std::unique_ptr<MediaContainer> vmapFrame(new VMapFrame(ctrl));
    vmapFrame->setGeometry(0, 0, 800, 600);
    addWidgetToMdi(vmapFrame.get(), ctrl->name());
    m_mediaContainers.push_back(std::move(vmapFrame));
}
void Workspace::addCharacterSheet(CharacterSheetController* ctrl)
{
    if(nullptr == ctrl)
        return;
    std::unique_ptr<CharacterSheetWindow> window(new CharacterSheetWindow(ctrl));
    addWidgetToMdi(window.get(), ctrl->name());
    m_mediaContainers.push_back(std::move(window));
}

void Workspace::addWebpage(WebpageController* ctrl)
{
    if(nullptr == ctrl)
        return;
    std::unique_ptr<WebView> window(new WebView(ctrl));
    addWidgetToMdi(window.get(), ctrl->name());
    m_mediaContainers.push_back(std::move(window));
}

void Workspace::addNote(NoteController* ctrl)
{
    if(nullptr == ctrl)
        return;
    std::unique_ptr<NoteContainer> SharedNote(new NoteContainer(ctrl));
    SharedNote->setGeometry(0, 0, 800, 600);
    addWidgetToMdi(SharedNote.get(), ctrl->name());
    m_mediaContainers.push_back(std::move(SharedNote));
}

void Workspace::addSharedNote(SharedNoteController* ctrl)
{
    if(nullptr == ctrl)
        return;
    std::unique_ptr<SharedNoteContainer> SharedNote(new SharedNoteContainer(ctrl));
    SharedNote->setGeometry(0, 0, 800, 600);
    addWidgetToMdi(SharedNote.get(), ctrl->name());
    m_mediaContainers.push_back(std::move(SharedNote));
}
#ifdef WITH_PDF
void Workspace::addPdf(PdfController* ctrl)
{
    if(nullptr == ctrl)
        return;
    std::unique_ptr<PdfViewer> pdfViewer(new PdfViewer(ctrl));
    addWidgetToMdi(pdfViewer.get(), ctrl->name());
    m_mediaContainers.push_back(std::move(pdfViewer));
}
#endif
