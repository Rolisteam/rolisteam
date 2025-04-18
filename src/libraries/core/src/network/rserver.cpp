#include "network/rserver.h"
#include <QDebug>
#include <QMessageLogger>
#include <network/serverconnectionmanager.h>

#include "network/timeaccepter.h"
#include "network/upnp/upnpnat.h"

RServer::RServer(const QMap<QString, QVariant>& parameter, bool internal, QObject* parent)
    : QTcpServer(parent), m_corConnection(new TimeAccepter()), m_data(parameter), m_internal(internal)
{
    if(m_data.contains("ThreadCount"))
        m_threadCount= m_data.value("ThreadCount", m_threadCount).toInt();
    connect(this, &RServer::portChanged, this, &RServer::runUpnpNat);
    runUpnpNat();
}

RServer::~RServer()
{
    emit eventOccured(QStringLiteral("Server Destructor"), LogController::Debug);
    for(auto& info : m_threadPool)
    {
        info.m_thread->quit();
        info.m_thread->wait(1000);
    }
}

void RServer::runUpnpNat()
{
    auto nat= new UpnpNat(this);
    nat->init(5, 10);
    connect(nat, &UpnpNat::discoveryEnd, this,
            [this, nat](bool b)
            {
                qDebug() << "discover ends" << nat;
                if(b)
                    nat->addPortMapping("Roliserver", nat->localIp(), m_port, m_port, "TCP");
            });
    connect(nat, &UpnpNat::statusChanged, this,
            [this, nat]()
            {
                if(nat->status() == UpnpNat::NAT_STAT::NAT_ADD)
                {
                    emit eventOccured(tr("[Upnp] Port mapping has been done"), LogController::LogLevel::Features);
                    nat->deleteLater();
                }
            });

    connect(nat, &UpnpNat::lastErrorChanged, this,
            [this, nat]() { emit eventOccured(nat->lastError(), LogController::LogLevel::Error); });

    nat->discovery();
}

bool RServer::listen()
{
    if(!QTcpServer::listen(QHostAddress::Any, m_port))
    {
        setState(RServer::Error);
        return false;
    }
    emit eventOccured(QStringLiteral("Start Listening"), LogController::Info);

    m_connectionsManager.reset(new ServerConnectionManager(m_data));
    m_updater.reset(new ServerManagerUpdater(m_connectionsManager.get(), m_internal));
    m_connectionThread.reset(new QThread);

    connect(this, &RServer::finished, m_connectionsManager.get(), &ServerConnectionManager::quit, Qt::QueuedConnection);
    connect(this, &RServer::accepting, m_connectionsManager.get(), &ServerConnectionManager::accept,
            Qt::QueuedConnection);
    connect(m_connectionsManager.get(), &ServerConnectionManager::finished, this, &RServer::complete,
            Qt::QueuedConnection);

    for(int i= 0; i < m_threadCount; ++i)
    {
        ThreadInfo info{new QThread(this), 0};
        m_threadPool.append(info);
        info.m_thread->start();
    }

    m_connectionsManager->moveToThread(m_connectionThread.get());
    m_updater->moveToThread(m_connectionThread.get());

    m_connectionThread->start();

    setState(RServer::Listening);

    return true;
}

void RServer::close()
{
    emit finished();
    QTcpServer::close();
    setState(RServer::Stopped);
}

qint64 RServer::port()
{
    return m_port;
}

bool RServer::internal() const
{
    return m_internal;
}

int RServer::threadCount() const
{
    return m_threadCount;
}

RServer::ServerState RServer::state() const
{
    return m_state;
}

void RServer::setPort(int p)
{
    if(p == m_port)
        return;
    m_port= p;
    emit portChanged();
}

void RServer::incomingConnection(qintptr descriptor)
{
    emit eventOccured(QStringLiteral("incoming Connection"), LogController::Debug);
    if(!m_corConnection->runAccepter(m_data))
        return;

    ServerConnection* connection= new ServerConnection(nullptr, nullptr);
    accept(descriptor, connection);
}

void RServer::accept(qintptr descriptor, ServerConnection* connection)
{
    if(m_threadPool.isEmpty())
    {
        emit eventOccured(QStringLiteral("threadpool is empty - server is stopped"), LogController::Error);
        return;
    }

    auto it= std::min_element(std::begin(m_threadPool), std::end(m_threadPool),
                              [](const ThreadInfo& a, const ThreadInfo& b)
                              { return a.m_connectionCount < b.m_connectionCount; });

    if(it == std::end(m_threadPool))
    {
        emit eventOccured(QStringLiteral("No thread available - server is stopped"), LogController::Error);
        return;
    }

    it->m_connectionCount++;
    connection->moveToThread(it->m_thread);
    emit accepting(descriptor, connection);
}

void RServer::setState(const ServerState& state)
{
    if(state == m_state)
        return;
    m_state= state;
    emit stateChanged(m_state);

    emit eventOccured(QStringLiteral("State Changed to %1").arg(state), LogController::Info);

    if(m_state == Error)
        emit eventOccured(errorString(), LogController::Error);
}

void RServer::complete()
{
    if(!m_connectionThread)
    {
        emit eventOccured(QStringLiteral("exiting complete there was no thread!"), LogController::Warning);
        return;
    }

    m_connectionsManager.release();

    qDebug() << this << "Quitting thread";
    m_connectionThread->quit();
    m_connectionThread->wait();

    m_connectionThread.release();

    emit eventOccured(QStringLiteral("Rserver Complete"), LogController::Info);
    emit completed();
}
