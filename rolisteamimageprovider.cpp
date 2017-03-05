#include "rolisteamimageprovider.h"
#include <QDebug>
#include <QBuffer>

QHash<QString,QPixmap>* RolisteamImageProvider::s_data = new QHash<QString,QPixmap>();


RolisteamImageProvider::RolisteamImageProvider()
    :  QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

RolisteamImageProvider::~RolisteamImageProvider()
{
}

QPixmap RolisteamImageProvider::requestPixmap(const QString &id, QSize *size, const QSize& requestedSize)
{
    /// @warning Ugly stuff
    //qDebug() << "Image Provider" << s_data->size();
    QString idTranslate = id;
    idTranslate = idTranslate.replace("%7B","{").replace("%7D","}");

    QPixmap pixmap= s_data->value(idTranslate);

    if (NULL!=size)
        return pixmap.copy(0,0,size->width(),size->height());
    else
        return pixmap;
}

void RolisteamImageProvider::insertPix(QString key, QPixmap img)
{
   // qDebug() << key << img << "Image Provider";
    s_data->insert(key,img);
}
QHash<QString,QPixmap>* RolisteamImageProvider::getData()
{
    return s_data;
}
void RolisteamImageProvider::cleanData()
{
    if(nullptr != s_data)
    {
        s_data->clear();
    }
}

void RolisteamImageProvider::setData( QHash<QString, QPixmap> * data)
{
    s_data = data;
}
#ifndef RCSE
void RolisteamImageProvider::fill(NetworkMessageWriter &msg)
{
    msg.uint16(s_data->count());
    for(auto key : s_data->keys())
    {
        msg.string16(key);
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        s_data->value(key).toImage().save(&buffer,"jpg");
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
        insertPix(key,pix);
    }
}
#endif
