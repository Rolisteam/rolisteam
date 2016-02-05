#ifndef ROLISTEAMIMAGEPROVIDER_H
#define ROLISTEAMIMAGEPROVIDER_H

#include <QQuickImageProvider>

class RolisteamImageProvider : public QQuickImageProvider
{
public:
    RolisteamImageProvider();
    //virtual QImage      requestImage(const QString & id, QSize * size, const QSize & requestedSize, bool requestedAutoTransform);
    virtual QPixmap 	requestPixmap(const QString & id, QSize * size, const QSize & requestedSize);
   // virtual QQuickTextureFactory * 	requestTexture(const QString & id, QSize * size, const QSize & requestedSize, bool requestedAutoTransform);

    virtual void insertPix(QString key,QPixmap img);
private:
    QHash<QString,QPixmap> m_data;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
