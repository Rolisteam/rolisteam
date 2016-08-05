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
    virtual ~RolisteamImageProvider();
    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize& requestedSize);

    static void insertPix(QString key,QPixmap img);
    static QHash<QString,QPixmap>* getData();


    QHash<QString, QPixmap> data() const;
    void setData(const QHash<QString, QPixmap> &data);

#ifndef RCSE
    void fill(NetworkMessageWriter& msg);
    void read(NetworkMessageReader& msg);
#endif
private:
    static QHash<QString,QPixmap>* s_data;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
