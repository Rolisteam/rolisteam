#include "rolisteamimageprovider.h"


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

