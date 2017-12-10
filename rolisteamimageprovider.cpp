#include "rolisteamimageprovider.h"
#include <QDebug>
#include <QBuffer>



RolisteamImageProvider::RolisteamImageProvider()
    :  QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    m_data.reset(new QHash<QString,QPixmap>());
}

RolisteamImageProvider::~RolisteamImageProvider()
{
}

QPixmap RolisteamImageProvider::requestPixmap(const QString &id, QSize *size, const QSize& requestedSize)
{
      Q_UNUSED(requestedSize);
    /// @warning Ugly stuff
    //qDebug() << "Image Provider" << s_data->size();
    QString idTranslate = id;
    idTranslate = idTranslate.replace("%7B","{").replace("%7D","}");

    QPixmap pixmap= m_data->value(idTranslate);
 /*   if(nullptr == pixmap)
    {
        return QPixmap();
    }*/

    if (nullptr!=size)
    {
        return pixmap.copy(0,0,size->width(),size->height());
    }
    else
    {
        return pixmap;
    }
}

void RolisteamImageProvider::insertPix(QString key, QPixmap img)
{
    m_data->insert(key,img);
}
QSharedPointer<QHash<QString,QPixmap>> RolisteamImageProvider::getData()
{
    return m_data;
}
void RolisteamImageProvider::cleanData()
{
    m_data->clear();
}

void RolisteamImageProvider::setData( QSharedPointer<QHash<QString, QPixmap>> data)
{
    m_data = data;
}
#ifndef RCSE
void RolisteamImageProvider::fill(NetworkMessageWriter &msg)
{
    msg.uint16(m_data->count());
    for(auto key : m_data->keys())
    {
        msg.string16(key);
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        m_data->value(key)->toImage().save(&buffer,"jpg");
        msg.byteArray32(array);
    }
}

void RolisteamImageProvider::read(NetworkMessageReader &msg)
{
    int size = msg.uint16();
    for(int i = 0; i<size; ++i)
    {
        QString key = msg.string16();
        QByteArray array = msg.byteArray32();
        QPixmap* pix = new QPixmap();
        pix->loadFromData(array);
        insertPix(key,pix);
    }
}
#endif
void RolisteamImageProvider::removeImg(QString key)
{
    m_data->remove(key);
}
