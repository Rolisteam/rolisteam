#include "pluginmanager.h"
#include "dicesysteminterface.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QtGui>

DicePlugInManager::DicePlugInManager(QObject *parent) :
    QObject(parent)
{
    m_interfaceList = new QList<DiceSystemInterface*>;
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
       QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
       QObject *plugin = loader.instance();
       if (plugin)
       {
           DiceSystemInterface * op = qobject_cast<DiceSystemInterface *>(plugin);
           if (op)
           {
              m_interfaceList->append(op);
           }
       }
       else
           qDebug() << loader.errorString();
    }

}
DicePlugInManager::~DicePlugInManager()
{

}
DicePlugInManager* DicePlugInManager::m_singleton = NULL;

DicePlugInManager* DicePlugInManager::instance()
{
    if(!m_singleton)
       m_singleton = new DicePlugInManager();

    return m_singleton;
}

DiceSystemInterface* DicePlugInManager::getInterface(QString& name)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        if(tmp->getName()==name)
            return tmp;
    }
    return NULL;
}
QStringList DicePlugInManager::getInterfaceList()
{
    QStringList interfaceNames;
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        interfaceNames << tmp->getName();
    }
    return interfaceNames;
}
void DicePlugInManager::readSettings()
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        tmp->readSettings();
    }
}

void DicePlugInManager::writeSettings()
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        tmp->writeSettings();
    }
}
void DicePlugInManager::showUISettings(QString& name)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        if(name==tmp->getName())
            tmp->showUiSettings();
    }
}

bool DicePlugInManager::hasUISettings(QString& name)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        if(name==tmp->getName())
           return tmp->hasUiSettings();
    }
}
