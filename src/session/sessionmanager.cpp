#include "sessionmanager.h"
#include <QTreeView>
#include <QHBoxLayout>

#include "sessionitemmodel.h"
#include "session.h"

SessionManager::SessionManager()
{
    m_currentSession = new Session();
    setObjectName("SessionManager");
    m_view =new QTreeView;
    m_layout = new QHBoxLayout;
    m_layout->addWidget(m_view);
    m_internal = new QWidget();
    m_internal->setLayout(m_layout);
    setWidget(m_internal);

    setWindowTitle(tr("Resources Explorer"));


    m_model = new SessionItemModel;
    m_view->setModel(m_model);
}
Chapter* SessionManager::getCurrentChapter()
{
    return NULL;
}
void SessionManager::setCurrentSession(Session* s)
{
    m_currentSession=s;
    /// @todo test code, should be removed
    QString t = tr("Chapter 1");
    m_currentSession->addChapter(t);
    m_model->setSession(m_currentSession);
}
CleverURI* SessionManager::addRessource(QString& urifile, CleverURI::ContentType type)
{
    QModelIndex index = m_view->currentIndex();
    return m_model->addRessources(urifile,type,index);
}

void SessionManager::readSettings(QSettings & m)
{
    m.beginGroup("Session");
    QVariant r;
    r.setValue<Session>(*m_currentSession);
    *m_currentSession=m.value("Session",r).value<Session>();
    m.endGroup();


}

void SessionManager::writeSettings(QSettings & m)
{
    m.beginGroup("Session");
    QVariant r;
    r.setValue<Session>(*m_currentSession);
    m.setValue("Session",r);
    m.endGroup();
}
