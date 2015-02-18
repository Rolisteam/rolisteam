#include "pluginmanager.h"
#include "dicesysteminterface.h"


DicePlugInManager::DicePlugInManager(QObject *parent) :
    QObject(parent)
{
    m_interfaceList = new QList<DiceSystemInterface*>;
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
