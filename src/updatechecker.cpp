#include "updatechecker.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>


UpdateChecker::UpdateChecker()
{
    m_state = false;
}
bool UpdateChecker::mustBeUpdated()
{
    return m_state;
}

void UpdateChecker::startChecking()
{
#ifdef VERSION
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(readXML(QNetworkReply*)));

     m_manager->get(QNetworkRequest(QUrl("http://www.rolisteam.org/version.xml")));
#endif
}

QString& UpdateChecker::getLatestVersion()
{
    return m_version;
}

QString& UpdateChecker::getLatestVersionDate()
{
    return m_versionDate;
}
void UpdateChecker::readXML(QNetworkReply* p)
{
#ifdef VERSION
    QByteArray a= p->readAll();

    QRegExp version("<version>(.*)</version>");
    QRegExp date("<date>(.*)</date>");

    QString string(a);
    int pos = version.indexIn(string);
    if(pos!=-1)
    {
        m_version = version.capturedTexts().at(1);
    }
     pos = date.indexIn(string);
     if(pos!=-1)
     {
          m_versionDate = date.capturedTexts().at(1);
     }

     if(VERSION!=m_version)
         m_state=true;

    emit checkFinished();
#endif
}
