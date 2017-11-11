#ifndef GAMEMASTERTOOL_H
#define GAMEMASTERTOOL_H

#include <QWidget>
#include <QSettings>

class GameMasterTool : public QWidget
{
    Q_OBJECT
public:
    explicit GameMasterTool(QWidget *parent = nullptr);

    virtual void readSettings(QSettings& settings);
    virtual void writeSettings(QSettings& settings);

};

#endif // GAMEMASTERTOOL_H
