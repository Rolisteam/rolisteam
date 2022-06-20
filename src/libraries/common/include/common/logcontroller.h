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
#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <QFile>
#include <QMutex>
#include <QObject>
#include <QTextStream>
#include <memory>

#include <common/common_global.h>

namespace helper
{
namespace log
{
COMMON_EXPORT QString humanReadableDiceResult(const QString& json);
}
} // namespace helper
/**
 * @brief The LogController class receives log messeges and displays them in the right sink.
 */
class COMMON_EXPORT LogController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LogController::LogLevel logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
public:
    enum LogLevel
    {
        Features= -1,
        Error= 0,
        Warning,
        Info,
        Debug,
        Hidden,
        Search,
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
    QString currentPath() const;
    void setSignalInspection(bool signalInspection);
    void setListenOutSide(bool);

    void setMessageHandler(bool attachMessage);
    static QString typeToText(LogController::LogLevel type);

    void setCurrentPath(const QString& path);

signals:
    void showMessage(QString, LogController::LogLevel);
    void sendOffMessage(QString, QString, QString category, QString timestamps);
    void logLevelChanged();
    void currentPathChanged();

public slots:
    void manageMessage(QString message, LogController::LogLevel type);
    void listenObjects(const QObject* widget);
    void signalActivated();
    void actionActivated();
    void setLogLevel(const LogController::LogLevel& logLevel);
    void setCurrentModes(const LogController::StorageModes& currentModes);
    void logToFile(const QString& msg, const LogController::LogLevel& type, const QString& log);

private:
    LogLevel m_logLevel= Error;
    StorageModes m_currentModes= Console;
    bool m_signalInspection= false;
    bool m_listenOutSide= false;
    QString m_currentFile;
    QMutex m_mutex;
    bool m_streamUp= false;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(LogController::StorageModes)
#endif // LOGCONTROLLER_H
