#include "mediacontainer.h"
#include <QMessageBox>


MediaContainer::MediaContainer(QWidget* parent)
    : QMdiSubWindow(parent),m_uri(NULL),m_preferences(PreferencesManager::getInstance())
{
    //m_preferences = ;
    setAttribute(Qt::WA_DeleteOnClose,false);
}
MediaContainer::~MediaContainer()
{
}

void MediaContainer::setLocalPlayerId(QString id)
{
    m_localPlayerId = id;
}

QString MediaContainer::getLocalPlayerId()
{
    return m_localPlayerId;
}
void MediaContainer::setCleverUri(CleverURI* uri)
{
	m_uri = uri;
}
QString MediaContainer::getTitle() const
{
	return m_title;
}
CleverURI*  MediaContainer::getCleverUri() const
{
    return  m_uri;
}
void MediaContainer::openMedia()
{

}
void MediaContainer::error(QString err,QWidget* parent)
{
    if(NULL!=parent)
    {
        QMessageBox msgBox(parent);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QObject::tr("Loading error"));
        msgBox.move(QPoint(parent->width()/2, parent->height()/2) + QPoint(-100, -50));

        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        msgBox.setText(err);
        msgBox.exec();
    }
}
bool MediaContainer::isUriEndWith(QString str)
{
    if(NULL!=m_uri)
    {
        if(m_uri->getUri().endsWith(str))
        {
            return true;
        }
    }
    return false;
}

void MediaContainer::setTitle(QString str)
{
  m_title = str;
  setWindowTitle(m_title);
}
void MediaContainer::setVisible(bool b)
{
    if(NULL!=widget())
    {
        widget()->setVisible(b);
        m_action->setChecked(b);
    }
    QMdiSubWindow::setVisible(b);
}
void MediaContainer::setAction(QAction* act)
{
    m_action = act;
    connect(m_action, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));
}
QAction* MediaContainer::getAction()
{
    return m_action;
}
