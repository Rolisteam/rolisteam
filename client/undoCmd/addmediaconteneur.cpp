#include "addmediaconteneur.h"
#include "improvedworkspace.h"
#include "network/networkmessagewriter.h"
#include <QDebug>
AddMediaConteneur::AddMediaConteneur(MediaContainer* mediac,
                                     SessionManager* manager,
                                     QMenu* menu,
                                     MainWindow* main,
                                     ImprovedWorkspace* workspace,
                                     bool gm,
                                     QUndoCommand* parent)
    : QUndoCommand(parent),
      m_media(mediac),
      m_manager(manager),
      m_menu(menu),
      m_main(main),
      m_gm(gm),
      m_mdiArea(workspace)
{
    setText(QObject::tr("Show %1").arg(mediac->getTitle()));
}

void AddMediaConteneur::redo()
{
    if(nullptr != m_media)
    {
        //add in workspace + add action and add into ressources manager.
        CleverURI* uri = m_media->getCleverUri();
        if(nullptr!=uri)
        {
            m_main->setLatestFile(uri);
            m_manager->addRessource(m_media->getCleverUri());
            uri->setDisplayed(true);
        }
        QAction *action = m_menu->addAction(m_media->getTitle());
        action->setCheckable(true);
        action->setChecked(true);
        m_media->setAction(action);
        m_mdiArea->addContainerMedia(m_media);
        m_media->setVisible(true);
        m_media->setFocus();

        if(sendAtOpening())
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::addMedia);
            qDebug() << "########################\n" << uri->getType();
            msg.int8(static_cast<int>(uri->getType()));
            m_media->fill(msg);
            msg.sendAll();
        }
    }
}

void AddMediaConteneur::undo()
{
    //remove from workspace, action in menu and from resources manager.
    if(nullptr != m_media)
    {
        m_menu->removeAction(m_media->getAction());
        m_manager->removeResource(m_media->getCleverUri());
        m_mdiArea->removeSubWindow(m_media);
        if(m_gm)
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::closeMedia);
            msg.string8(m_media->getMediaId());
            msg.sendAll();
        }
    }
}
bool AddMediaConteneur::sendAtOpening()
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
