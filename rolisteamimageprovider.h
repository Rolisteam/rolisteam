#ifndef ROLISTEAMIMAGEPROVIDER_H
#define ROLISTEAMIMAGEPROVIDER_H

#include <QQuickImageProvider>
#ifndef RCSE
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#endif
/**
 * @brief The RolisteamImageProvider class is providing images to qml engine for charactersheet.
 */
class RolisteamImageProvider : public QQuickImageProvider
{
public:
    RolisteamImageProvider();
    RolisteamImageProvider(const RolisteamImageProvider& copy);
    virtual ~RolisteamImageProvider();
    //virtual QImage      requestImage(const QString & id, QSize * size, const QSize & requestedSize, bool requestedAutoTransform);
    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize& requestedSize);
   // virtual QQuickTextureFactory * 	requestTexture(const QString & id, QSize * size, const QSize & requestedSize, bool requestedAutoTransform);

    virtual void insertPix(QString key,QPixmap img);


    QHash<QString, QPixmap> data() const;
    void setData(const QHash<QString, QPixmap> &data);
#ifndef RCSE
    void fill(NetworkMessageWriter& msg);
    void read(NetworkMessageReader& msg);
#endif
private:
    QHash<QString,QPixmap> m_data;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
