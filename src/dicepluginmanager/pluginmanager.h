#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>

#include <QList>
#include <QStringList>

class DiceSystemInterface;
class DicePlugInManager : public QObject
{
    Q_OBJECT
public:
    ~DicePlugInManager();
    static DicePlugInManager* instance();
    DiceSystemInterface* getInterface(QString& name);
    QStringList getInterfaceList();


private:
    DicePlugInManager(QObject *parent = 0);

    static DicePlugInManager* m_singleton;

    QList<DiceSystemInterface*>* m_interfaceList;
};

#endif // PLUGINMANAGER_H
