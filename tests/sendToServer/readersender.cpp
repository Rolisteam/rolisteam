#include "readersender.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QTcpSocket>
#include <chrono>
#include <thread>

ReaderSender::ReaderSender(QObject* parent) : QObject(parent) {}
void ReaderSender::readFile()
{
    QString path= QStringLiteral("/home/renaud/www/%1").arg("DataServer_2018-08-05T15:59:37.log");
    m_file.setFileName(path);
    if(m_file.open(QIODevice::ReadOnly))
    {
        m_reader.setDevice(&m_file);
    }

    while(!m_reader.atEnd())
    {
        m_data << m_reader.readLine();
    }

    sendData();
}
void ReaderSender::sendData()
{
    for(auto line : m_data)
    {
        auto array= line.split("_", QString::SkipEmptyParts);
        auto date= array[0];
        auto id= array[1].toInt();
        auto data= array[2];

        if(!m_hash.contains(id))
        {
            QTcpSocket* tcp= new QTcpSocket();

            tcp->connectToHost(QStringLiteral("127.0.0.1"), 6660);
            if(tcp->waitForConnected())
            {
                qDebug() << "Connected";
                m_hash.insert(id, tcp);
            }
            else
            {
                qDebug() << "No server";
                emit finish();
                return;
            }
        }
        auto tcp= m_hash[id];
        if(nullptr == tcp)
            return;

        QByteArray bytearray;
        bytearray= QByteArray::fromBase64(data.toLatin1());

        tcp->write(bytearray);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    emit finish();
    //  qApp->quit();
}
