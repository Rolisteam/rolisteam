#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <QFile>
#include <QMutex>
#include <QObject>
#include <QTextStream>
#include <memory>
/**
 * @brief The LogController class receives log messeges and displays them in the right sink.
 */
class LogController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LogController::LogLevel logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
public:
    enum LogLevel
    {
        Error,
        Debug,
        Warning,
        Info,
        Features,
        Hidden,
        Search
    };
    Q_ENUM(LogLevel)

    enum StorageMode
    {
        Console= 1,
        File= 2,
        Gui= 4,
        Network= 8
    };
    Q_ENUM(StorageMode)
    Q_DECLARE_FLAGS(StorageModes, StorageMode)
    Q_FLAGS(StorageModes)

    explicit LogController(bool attachMessage, QObject* parent= nullptr);
    ~LogController();

    LogController::StorageModes currentModes() const;
    LogController::LogLevel logLevel() const;

    bool signalInspection() const;
    void setSignalInspection(bool signalInspection);
    void setListenOutSide(bool);

    void setMessageHandler(bool attachMessage);
    static QString typeToText(LogController::LogLevel type);

    void setLogPath(const QString& path);

signals:
    void showMessage(QString, LogController::LogLevel);
    void sendOffMessage(QString, LogController::LogLevel, QString category, QString timestamps);
    void logLevelChanged();

public slots:
    void manageMessage(QString message, LogController::LogLevel type);
    void listenObjects(const QObject* widget);
    void signalActivated();
    void actionActivated();
    void setLogLevel(const LogController::LogLevel& logLevel);
    void setCurrentModes(const LogController::StorageModes& currentModes);

private:
    LogLevel m_logLevel= Error;
    StorageModes m_currentModes= Console;
    bool m_signalInspection= false;
    bool m_listenOutSide= false;
    QTextStream m_file;
    QFile m_currentFile;
    QMutex m_mutex;
    bool m_streamUp= false;
    std::unique_ptr<QFile> m_logfile;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(LogController::StorageModes)
#endif // LOGCONTROLLER_H
