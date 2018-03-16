#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <QObject>
#include <QTextStream>
#include <QFile>

class LogController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LogController::LogLevel logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
public:
    enum LogLevel {Error,Debug,Warning,Info};
    enum StorageMode {Console=0,File=1,Gui=2,Network=4,Database=8};
    Q_ENUM(LogLevel)
    Q_ENUM(StorageMode)
    explicit LogController(QObject *parent = nullptr);
    ~LogController();

    LogController::StorageMode currentModes() const;
    LogController::LogLevel logLevel() const;

    void manageMessage(QString message, LogController::LogLevel type);
signals:
    void showMessage(QString);
    void sendOffMessage(QString);
    void logLevelChanged();
public slots:
    void listenObjects(const QObject* widget);
    void signalActivated();
    void actionActivated();
    void setLogLevel(const LogLevel& logLevel);
    void setCurrentModes(const StorageMode &currentModes);

private:
    QString typeToText(LogController::LogLevel type);
private:
    LogLevel m_logLevel = Error;
    StorageMode m_currentModes = Console;
    bool m_signalInspection = false;
    QTextStream m_file;
    QFile m_currentFile;
};

#endif // LOGCONTROLLER_H
