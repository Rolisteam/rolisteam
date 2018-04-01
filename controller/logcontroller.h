#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <QObject>
#include <QTextStream>
#include <QFile>

/**
 * @brief The LogController class receives log messeges and displays them in the right sink.
 */
class LogController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LogController::LogLevel logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
public:
    enum LogLevel {Error,Debug,Warning,Info,Features};
    Q_ENUM(LogLevel)

    enum StorageMode {Console=1,File=2,Gui=4,Network=8,Database=16};
    Q_ENUM(StorageMode)
    Q_DECLARE_FLAGS(StorageModes, StorageMode)
    Q_FLAGS(StorageModes)

    explicit LogController(bool attachMessage,QObject *parent = nullptr);
    ~LogController();

    LogController::StorageModes currentModes() const;
    LogController::LogLevel logLevel() const;

    void manageMessage(QString message, LogController::LogLevel type);
    bool signalInspection() const;
    void setSignalInspection(bool signalInspection);
    void setListenOutSide(bool );

signals:
    void showMessage(QString,LogController::LogLevel);
    void sendOffMessage(QString);
    void logLevelChanged();
public slots:
    void listenObjects(const QObject* widget);
    void signalActivated();
    void actionActivated();
    void setLogLevel(const LogLevel& logLevel);
    void setCurrentModes(const StorageModes &currentModes);

private:
    QString typeToText(LogController::LogLevel type);
private:
    LogLevel m_logLevel = Error;
    StorageModes m_currentModes = Console;
    bool m_signalInspection = false;
    bool m_listenOutSide = false;
    QTextStream m_file;
    QFile m_currentFile;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(LogController::StorageModes)
#endif // LOGCONTROLLER_H
