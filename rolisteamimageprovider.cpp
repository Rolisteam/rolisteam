#include "rolisteamimageprovider.h"
#include <QBuffer>
#include <QDebug>

RolisteamImageProvider::RolisteamImageProvider(ImageModel* model) : QQuickImageProvider(QQuickImageProvider::Pixmap), m_model(model)
{
}

RolisteamImageProvider::~RolisteamImageProvider() {}

QPixmap RolisteamImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(requestedSize);
    QString idTranslate= id;
    idTranslate= idTranslate.replace("%7B", "{").replace("%7D", "}");

    QPixmap pixmap= m_model->pixmapFromKey(idTranslate);

    if(nullptr != size)
    {
        return pixmap.copy(0, 0, size->width(), size->height());
    }
    else
    {
        return pixmap;
    }
}

#ifndef RCSE
void RolisteamImageProvider::fill(NetworkMessageWriter& msg)
{
    msg.uint16(m_data->count());
    auto const& keys= m_data->keys();
    for(auto& key : keys)
    {
        msg.string16(key);
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        m_data->value(key).toImage().save(&buffer, "jpg");
        msg.byteArray32(array);
    }
}

void RolisteamImageProvider::read(NetworkMessageReader& msg)
{
    int size= msg.uint16();
    for(int i= 0; i < size; ++i)
    {
        QString key= msg.string16();
        QByteArray array= msg.byteArray32();
        QPixmap* pix= new QPixmap();
        pix->loadFromData(array);
        insertPix(key, *pix);
    }
}
#endif
