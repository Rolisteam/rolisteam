#ifndef ROLISTEAMIMAGEPROVIDER_H
#define ROLISTEAMIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QSharedPointer>
#include "imagemodel.h"

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
    RolisteamImageProvider(ImageModel* model);
    virtual ~RolisteamImageProvider();
    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);

#ifndef RCSE
    void fill(NetworkMessageWriter& msg);
    void read(NetworkMessageReader& msg);
#endif
private:
    ImageModel* m_model;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
