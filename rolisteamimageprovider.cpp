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

