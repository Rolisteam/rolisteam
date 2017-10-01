#include "deletemediacontainercommand.h"
#include "session/sessionmanager.h"
#include "mainwindow.h"
#include "network/networkmessagewriter.h"
#include "improvedworkspace.h"


DeleteMediaContainerCommand::DeleteMediaContainerCommand(MediaContainer* media, SessionManager* manager,QMenu* menu, MainWindow* main,ImprovedWorkspace* workspace,bool isGM, QUndoCommand* parent)
    : QUndoCommand (parent),
      m_media(media),
      m_manager(manager),
      m_menu(menu),
      m_main(main),
      m_mdiArea(workspace),
      m_gm(isGM)
{
    setText(QObject::tr("Close %1").arg(m_media->getTitle()));
}

void DeleteMediaContainerCommand::redo()
{
    if(nullptr != m_media)
    {
        m_menu->removeAction(m_media->getAction());
        auto uri = m_media->getCleverUri();
        if(nullptr != uri)
        {
            uri->setState(CleverURI::Remain);
        }
        m_manager->resourceClosed(m_media->getCleverUri());
        m_mdiArea->removeSubWindow(m_media);
        if(m_gm)
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::closeMedia);
            msg.string8(m_media->getMediaId());
            msg.sendAll();
        }
    }
}

void DeleteMediaContainerCommand::undo()
{
    if(nullptr != m_media)
    {
        CleverURI* uri = m_media->getCleverUri();
        if(nullptr!=uri)
        {
            m_main->setLatestFile(uri);
            m_manager->addRessource(m_media->getCleverUri());
            uri->setDisplayed(true);
        }
        QAction* action= m_media->getAction();
        if(action == nullptr)
        {
            action = m_menu->addAction(m_media->getTitle());
            action->setCheckable(true);
            action->setChecked(true);
        }
        m_media->setAction(action);
        m_mdiArea->addContainerMedia(m_media);
        m_media->setVisible(true);
        m_media->setFocus();
        if(sendAtOpening())
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::addMedia);
            msg.uint8(static_cast<quint8>(uri->getType()));
            m_media->fill(msg);
            msg.sendAll();
        }
    }
}
bool DeleteMediaContainerCommand::sendAtOpening()
{
    if(m_media->isRemote())
    {
       return false;
    }
    auto uri = m_media->getCleverUri();
    if(nullptr == uri)
    {
        return false;
    }
    bool result = false;
    switch(uri->getType())
    {
    case CleverURI::PICTURE:
    case CleverURI::VMAP:
    case CleverURI::MAP:
    case CleverURI::ONLINEPICTURE:
        result = true;
        break;
    default:
        result = false;
        break;
    }
    return result;
}
