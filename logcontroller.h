#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <QObject>
#include <QTextStream>
#include <QFile>

class LogController : public QObject
{
    Q_OBJECT
public:
    enum LogLevel {Error,Debug,Warning,Info};
    enum StorageMode {Console=0,File=1,Gui=2,Network=4,Database=8};
    Q_ENUM(LogLevel)
    Q_ENUM(StorageMode)
    explicit LogController(QObject *parent = nullptr);

    LogController::StorageMode currentModes() const;
    void setCurrentModes(const StorageMode &currentModes);

    LogController::LogLevel currentLevel() const;
    void setCurrentLevel(const LogLevel &currentLevel);


    void manageMessage(QString message, LogController::LogLevel type);
signals:
    void showMessage(QString);
    void sendOffMessage(QString);
public slots:
    void listenObjects(const QObject* widget);
    void signalActivated();
    void actionActivated();

private:
    QString typeToText(LogController::LogLevel type);
private:
    LogLevel m_currentLevel = Error;
    StorageMode m_currentModes = Console;
    bool m_signalInspection = false;
    QTextStream m_file;
    QFile m_currentFile;
};

#endif // LOGCONTROLLER_H
