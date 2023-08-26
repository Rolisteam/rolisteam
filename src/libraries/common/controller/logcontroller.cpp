/*************************************************************************
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#include <common/logcontroller.h>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMutexLocker>
#include <QTime>
#include <iostream>

#ifdef QT_WIDGETS_LIB
#include <QAction>
#include <QWidget>
#endif

LogController* controller= nullptr;

namespace helper
{
namespace log
{
QString humanReadableDiceResult(const QString& json)
{
    QJsonDocument doc= QJsonDocument::fromJson(json.toUtf8());
    auto obj= doc.object();

    QStringList list;
    std::transform(std::begin(obj), std::end(obj), std::back_inserter(list),
                   [obj](const QJsonValueRef& ref) { return ref.toString(); });
    /*auto command= obj[Core::jsonDice::JSON_COMMAND].toString();
    auto error= obj[Core::jsonDice::JSON_ERROR].toString();
    auto scalar= obj[Core::jsonDice::JSON_SCALAR].toString();*/

    list << QString(json.toUtf8().toBase64());
    return list.join(";");
}
} // namespace log
} // namespace helper

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    if(nullptr == controller)
        return;

    QByteArray localMsg= msg.toLocal8Bit();
    auto msgFormated= QStringLiteral("%1 (%2:%3), %4")
                          .arg(QString(localMsg.constData()), QString(context.file), QString::number(context.line),
                               QString(context.function));
    LogController::LogLevel cLevel= LogController::Error;
    switch(type)
    {
    case QtDebugMsg:
        cLevel= LogController::Debug;
        std::cout << msg.toStdString() << std::endl;
        break;
    case QtInfoMsg:
        cLevel= LogController::Info;
        std::cout << msg.toStdString() << std::endl;
        break;
    case QtWarningMsg:
        cLevel= LogController::Warning;
        std::cout << msg.toStdString() << std::endl;
        break;
    case QtCriticalMsg:
    case QtFatalMsg:
        cLevel= LogController::Error;
        std::cerr << msg.toStdString() << std::endl;
        break;
    }

    // controller->manageMessage(msgFormated,cLevel);
    QMetaObject::invokeMethod(controller, "manageMessage", Qt::QueuedConnection, Q_ARG(QString, msgFormated),Q_ARG(QString, QString::fromLocal8Bit(context.category)),
                              Q_ARG(LogController::LogLevel, cLevel));
    QMetaObject::invokeMethod(controller, "logToFile", Qt::QueuedConnection, Q_ARG(QString, msg),
                              Q_ARG(LogController::LogLevel, cLevel),
                              Q_ARG(QString, QString::fromLocal8Bit(context.category)));
}

LogController::LogController(bool attachMessage, QObject* parent) : QObject(parent)
{
    qRegisterMetaType<LogController::LogLevel>("LogController::LogLevel");
    setMessageHandler(attachMessage);
}

LogController::~LogController()
{
    controller= nullptr;
}

void LogController::setMessageHandler(bool attachMessage)
{
    //#ifndef QT_DEBUG
    if((controller == nullptr) && (attachMessage))
    {
        qInstallMessageHandler(messageHandler);
        controller= this;
    }
    else
    {
        qInstallMessageHandler(nullptr);
        controller= nullptr;
    }
    /*#else
        Q_UNUSED(attachMessage)
    #endif*/
}
LogController::StorageModes LogController::currentModes() const
{
    return m_currentModes;
}

void LogController::setCurrentModes(const StorageModes& currentModes)
{
    m_currentModes= currentModes;
}

LogController::LogLevel LogController::logLevel() const
{
    return m_logLevel;
}

void LogController::setLogLevel(const LogLevel& currentLevel)
{
    if(currentLevel == m_logLevel)
        return;

    m_logLevel= currentLevel;
    emit logLevelChanged();
}

void LogController::listenObjects(const QObject* object)
{
#ifdef QT_WIDGETS_LIB
    const auto widget= dynamic_cast<const QWidget*>(object);
    if(widget != nullptr)
    {
        QList<QAction*> actions= widget->actions();
        for(QAction* action : actions)
        {
            connect(action, &QAction::triggered, this, &LogController::actionActivated, Qt::QueuedConnection);
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
        QObjectList children= widget->children();
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
    auto act= qobject_cast<QAction*>(sender());
    manageMessage(QStringLiteral("[Action] - %1 - %2").arg(act->text(), act->objectName()), Info);
#endif
}

void LogController::signalActivated()
{
    auto obj= sender();
    auto index= senderSignalIndex();
    auto meta= obj->metaObject();
    auto method= meta->method(index);
    manageMessage(QStringLiteral("[signal] - %1").arg(QString::fromUtf8(method.name())), QStringLiteral("Signal"), Info);
}

QString LogController::typeToText(LogController::LogLevel type)
{
    static QHash<LogController::LogLevel, QString> list
        = {{LogController::Debug, "Debug"},  {LogController::Error, "Error"},      {LogController::Warning, "Warning"},
           {LogController::Info, "Info"},    {LogController::Features, "Feature"}, {LogController::Hidden, "Feature"},
           {LogController::Search, "Search"}};
    return list.value(type);
}

void LogController::setCurrentPath(const QString& path)
{
    std::cout << "path:" << path.toStdString() << std::endl;
    if(m_currentFile == path)
        return;
    m_currentFile= path;
    emit currentPathChanged();
}

bool LogController::signalInspection() const
{
    return m_signalInspection;
}

QString LogController::currentPath() const
{
    return m_currentFile;
}

void LogController::setSignalInspection(bool signalInspection)
{
    m_signalInspection= signalInspection;
}

void LogController::setListenOutSide(bool val)
{
    m_listenOutSide= val;
}

void LogController::manageMessage(QString message, const QString& category, LogController::LogLevel type)
{
    QMutexLocker locker(&m_mutex);
    Q_UNUSED(locker)

    QString str("%1 - %2 - %3");
    str= str.arg(QTime::currentTime().toString("hh:mm:ss"), typeToText(type), message);

    QString timestamps;
    timestamps= QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    if(type == Hidden)
    {
        emit showMessage(str, type);
        return;
    }

    if(type == Search || (m_currentModes & Network))
    {
        emit sendOffMessage(str, typeToText(type), category, timestamps);
    }

    if(type > m_logLevel)
        return;

    if(m_currentModes & Console)
    {
        if(type == Error)
            std::cerr << str.toStdString() << std::endl;
        else
            std::cout << str.toStdString() << std::endl;
    }

    if(m_currentModes & Gui)
    {
        emit showMessage(str, type);
    }
}

void LogController::logToFile(const QString& msg, const LogController::LogLevel& type, const QString& log)
{
    if(m_currentModes & File && !m_currentFile.isEmpty())
    {
        auto realMsg
            = QString("%1;%2;%3;%4")
                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), log, typeToText(type), msg);

        QFile output(m_currentFile);
        if(output.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            QTextStream text(&output);
            text << realMsg << "\n";
        }
    }
}
