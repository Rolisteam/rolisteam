#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QObject>
#include <QList>
#include <QStringList>

class DiceSystemInterface;
/**
  * @brief manages dice system plugin, it provides an API to run the dice command, and configure the plugin.
  */
class DicePlugInManager : public QObject
{
    Q_OBJECT
public:
    /**
      * @brief destructor
      */
    ~DicePlugInManager();
    /**
      * @brief instance provider, part of singleton pattern.
      */
    static DicePlugInManager* instance();
    /**
      * @brief accessor to the given interface.
      */
    DiceSystemInterface* getInterface(QString& name);
    /**
      * @brief accessor to the interface list
      */
    QStringList getInterfaceList();

    /**
      * @brief calls every interfaces to read their settings
      */
    void readSettings();

    /**
      * @brief calls every interfaces to write their settings.
      */
    void writeSettings();
    /**
      * @brief calls the same function to the given interface.
      */
    void showUISettings(QString& name);
    /**
      * @brief allows to know if the given interface has UI for settings
      */
    bool hasUISettings(QString& name);
private:
    DicePlugInManager(QObject *parent = 0);

    static DicePlugInManager* m_singleton;

    QList<DiceSystemInterface*>* m_interfaceList;
};

#endif // PLUGINMANAGER_H
