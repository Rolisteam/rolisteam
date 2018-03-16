#include "logcontroller.h"
#include <QAction>
#include <QMetaObject>
#include <QMetaMethod>
#include <QTime>

#ifdef QT_WIDGETS_LIB
#include <QWidget>
#endif

QTextStream out(stdout);
QTextStream err(stderr);
LogController* controller = nullptr;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(nullptr == controller)
        return;

    QByteArray localMsg = msg.toLocal8Bit();
    auto msgFormated = QStringLiteral("%1 (%2:%3), %4").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
    LogController::LogLevel cLevel = LogController::Error;
    switch (type) {
    case QtDebugMsg:
        cLevel = LogController::Debug;
        break;
    case QtInfoMsg:
        cLevel = LogController::Info;
        break;
    case QtWarningMsg:
        cLevel = LogController::Warning;
        break;
    case QtCriticalMsg:
    case QtFatalMsg:
        cLevel = LogController::Error;
        break;

    }

    controller->manageMessage(msgFormated,cLevel);
}

LogController::LogController(QObject *parent)
  : QObject(parent)
{
    if(controller == nullptr)
    {
        qInstallMessageHandler(messageHandler);
        controller = this;
    }
}

LogController::~LogController()
{
      controller = nullptr;
}

LogController::StorageMode LogController::currentModes() const
{
    return m_currentModes;
}

void LogController::setCurrentModes(const StorageMode &currentModes)
{
    m_currentModes = currentModes;
}

LogController::LogLevel LogController::logLevel() const
{
    return m_logLevel;
}

void LogController::setLogLevel(const LogLevel &currentLevel)
{
    if(currentLevel == m_logLevel)
        return;

    m_logLevel = currentLevel;
    emit logLevelChanged();
}

void LogController::listenObjects(const QObject* object)
{
#ifdef QT_WIDGETS_LIB
   const auto widget = dynamic_cast<const QWidget*>(object);
    if(widget != nullptr)
    {
        QList<QAction*> actions = widget->actions();
        for(QAction* action : actions)
        {
            connect(action,&QAction::triggered,this,&LogController::actionActivated);
        }
    }
#endif

    if(m_signalInspection)
    {
        auto metaObject = object->metaObject();

        for(int i = 0; i < metaObject->methodCount(); ++i)
        {
            auto meth = metaObject->method(i);
            if(meth.methodType() == QMetaMethod::Signal && meth.access() == QMetaMethod::Public)
            {
                connect(object,SIGNAL(meth.name()),this,SLOT(actionActivated()));
            }
        }
    }

    QObjectList children = widget->children();
    for(QObject* obj : children)
    {
        listenObjects(obj);
    }

}


void LogController::actionActivated()
{
    auto act = qobject_cast<QAction*>(sender());
    manageMessage(QStringLiteral("[Action] - %1 - %2").arg(act->text()).arg(act->objectName()),Info);
}

void LogController::signalActivated()
{
    auto obj = sender();
    auto index = senderSignalIndex();
    auto meta = obj->metaObject();
    auto method = meta->method(index);
    manageMessage(QStringLiteral("[signal] - %1").arg(QString::fromUtf8(method.name())),Info);
}
QString LogController::typeToText(LogController::LogLevel type)
{
    static QStringList list = {"Error","Debug","Warning","Info"};
    return list.at(type);
}

void LogController::manageMessage(QString message, LogController::LogLevel type)
{
    if(m_logLevel < type)
        return;

    QString str("%1 - %2 - %3");
    str=str.arg(QTime::currentTime().toString("hh:mm:ss")).arg(typeToText(type)).arg(message);


    if(m_currentModes & Console)
    {
        if(type == Error)
            err << str;
        else
            out << str;
    }
    if(m_currentModes & File)
    {
        m_file << str;
    }
    if(m_currentModes & Gui)
    {
        emit showMessage(str);
    }
    if(m_currentModes & Network)
    {
        emit sendOffMessage(str);
    }
    if(m_currentModes & Database)
    {
        emit sendOffMessage(str);
    }
}
