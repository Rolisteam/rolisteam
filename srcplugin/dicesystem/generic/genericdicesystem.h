#ifndef GENERICDICESYSTEM_H
#define GENERICDICESYSTEM_H

#include <QObject>
#include "dicesysteminterface.h"

/**
  * @brief GenericDiceSystem should provide an interpertor for new version of the dice syntax
  *
  */
class GenericDiceSystem : public QObject,public DiceSystemInterface
{
    Q_OBJECT

    Q_INTERFACES(DiceSystemInterface)
    Q_PLUGIN_METADATA(IID "org.rolisteam.genericdicesystem")

public:
    /**
     * @brief default constructor
     */
    GenericDiceSystem();
    /**
     * @brief destructor
     */
    ~GenericDiceSystem();
    /**
     * @brief accessor to the Name
     */
    virtual QString getName() const;
    /**
     * @brief Rolisteam calls the dice system interpretor and gives it the instruction line it should parse.
     * @param line to parse
     * @return the result of the dice rolls with appropriate text.
     */
    virtual QString rollDice(QString diceCmd) const;
    /**
     * @brief read the plugin settings
     */
    virtual void readSettings(QSettings& setting);
    /**
     * @brief write the plugin settings
     */
    virtual void writeSettings(QSettings& setting);

    /**
     * @brief Part of the dice system API, this plugin does not have GUI for its settings.
     *
     */
    virtual bool hasUiSettings() const;
    /**
     * @brief does nothing.
     */
    virtual void showUiSettings();

    virtual const QString& catchRegExp() const;
private:
    QString m_regexp;
};


#endif // GENERICDICESYSTEM_H
