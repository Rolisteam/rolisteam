#ifndef ROLISTEAMIMAGEPROVIDER_H
#define ROLISTEAMIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QSharedPointer>
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
    virtual ~RolisteamImageProvider();
    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);

    void insertPix(QString key, QPixmap img);
    QSharedPointer<QHash<QString, QPixmap>> getData();
    void cleanData();

    void setData(QSharedPointer<QHash<QString, QPixmap>> data);

    void removeImg(QString key);

#ifndef RCSE
    void fill(NetworkMessageWriter& msg);
    void read(NetworkMessageReader& msg);
#endif
private:
    QSharedPointer<QHash<QString, QPixmap>> m_data;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
