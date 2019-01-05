#include "logcontroller.h"
#include <QMetaObject>
#include <QMetaMethod>
#include <QTime>
#include <QDateTime>
#include <QMutexLocker>
#include <iostream>

#ifdef QT_WIDGETS_LIB
#include <QAction>
#include <QWidget>
#endif

QTextStream out(stdout, QIODevice::WriteOnly);
QTextStream err(stderr, QIODevice::WriteOnly);
LogController* controller = nullptr;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(nullptr == controller)
        return;

    QByteArray localMsg = msg.toLocal8Bit();
    auto msgFormated = QStringLiteral("%1 (%2:%3), %4").arg(QString(localMsg.constData()),QString(context.file),QString(context.line),QString(context.function));
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

    //controller->manageMessage(msgFormated,cLevel);
    QMetaObject::invokeMethod(controller,"manageMessage",Qt::QueuedConnection,Q_ARG(QString,msgFormated),Q_ARG(LogController::LogLevel,cLevel));
}

LogController::LogController(bool attachMessage,QObject *parent)
  : QObject(parent)
{
    qRegisterMetaType<LogController::LogLevel>("LogController::LogLevel");
    setMessageHandler(attachMessage);
}

LogController::~LogController()
{
      controller = nullptr;
}


void LogController::setMessageHandler(bool attachMessage)
{
#ifndef QT_DEBUG
    if((controller == nullptr) && (attachMessage))
    {
           qInstallMessageHandler(messageHandler);
           controller = this;
    }
    else
    {
        qInstallMessageHandler(nullptr);
        controller= nullptr;
    }
#else
    Q_UNUSED(attachMessage)
#endif
}
LogController::StorageModes LogController::currentModes() const
{
    return m_currentModes;
}

void LogController::setCurrentModes(const StorageModes &currentModes)
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
            connect(action,&QAction::triggered,this,&LogController::actionActivated,Qt::QueuedConnection);
        }
    }
#endif
    Q_UNUSED(object)
    /*if(m_signalInspection)
    {
        auto metaObject = object->metaObject();

        for(int i = 0; i < metaObject->methodCount(); ++i)
        {
            auto meth = metaObject->method(i);
           // qDebug() << meth.name();
            if(meth.methodType() == QMetaMethod::Signal && meth.access() == QMetaMethod::Public)
            {
                connect(object,meth.name(),this,SLOT(actionActivated()),Qt::QueuedConnection);
            }
        }
    }*/
#ifdef QT_WIDGETS_LIB
    if(widget != nullptr)
    {
        QObjectList children = widget->children();
        for(QObject* obj : children)
        {
            listenObjects(obj);
        }
    }
#endif
}


void LogController::actionActivated()
{
    #ifdef QT_WIDGETS_LIB
    auto act = qobject_cast<QAction*>(sender());
    manageMessage(QStringLiteral("[Action] - %1 - %2").arg(act->text(), act->objectName()),Info);
    #endif
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
    static QStringList list = {"Error","Debug","Warning","Info","Feature","Feature","Search"};
    return list.at(type);
}

bool LogController::signalInspection() const
{
    return m_signalInspection;
}

void LogController::setSignalInspection(bool signalInspection)
{
    m_signalInspection = signalInspection;
}

void LogController::setListenOutSide(bool val)
{
    m_listenOutSide = val;
}

void LogController::manageMessage(QString message, LogController::LogLevel type)
{
  /*  if(m_logLevel < type && type != Features && type != Hidden)
        return;*/

    QMutexLocker locker(&m_mutex);
    Q_UNUSED(locker)

    QString str("%1 - %2 - %3");
    str=str.arg(QTime::currentTime().toString("hh:mm:ss"),typeToText(type),message);

    QString timestamps;
    QString category;// WARNING unused
    timestamps = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    if(type != Search)
    {
        if(m_currentModes & Console || type == Debug || m_logLevel >= type || type == Error)
        {
            if(type == Error)
                std::cerr << str.toStdString() << std::endl;
            else
                std::cout << str.toStdString() << std::endl;
        }
        if(m_logLevel >= type || type == Features || type == Hidden)
        {
            if(m_currentModes & File)
            {
                m_file << str;
            }
            if(m_currentModes & Gui)
            {
                emit showMessage(str,type);
            }
        }
    }
    if((m_currentModes & Network) && (type != Hidden))
    {
        emit sendOffMessage(str,type,category,timestamps);
    }
}
