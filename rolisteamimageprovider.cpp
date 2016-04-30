#include "rolisteamimageprovider.h"
#include <QDebug>
#include <QBuffer>

RolisteamImageProvider::RolisteamImageProvider()
    :  QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

RolisteamImageProvider::~RolisteamImageProvider()
{

}

QPixmap RolisteamImageProvider::requestPixmap(const QString &id, QSize *size, const QSize& requestedSize)
{
    QPixmap pixmap= m_data.value(id);

    if (NULL!=size)
        return pixmap.copy(0,0,size->width(),size->height());
    else
        return pixmap;
}

void RolisteamImageProvider::insertPix(QString key, QPixmap img)
{
    m_data[key]=img;
}

QHash<QString, QPixmap> RolisteamImageProvider::data() const
{
    return m_data;
}

void RolisteamImageProvider::setData(const QHash<QString, QPixmap> &data)
{
    m_data = data;
}
#ifndef RCSE
void RolisteamImageProvider::fill(NetworkMessageWriter &msg)
{
    msg.uint16(m_data.count());
    for(auto key : m_data.keys())
    {
        msg.string16(key);
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        m_data[key].toImage().save(&buffer,"jpg");
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
        QPixmap pix;
        pix.loadFromData(array);
        m_data.insert(key,pix);
    }
}
#endif
