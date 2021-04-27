#ifndef ROLISTEAMIMAGEPROVIDER_H
#define ROLISTEAMIMAGEPROVIDER_H

#include "imagemodel.h"
#include <QQuickImageProvider>
#include <QSharedPointer>

/**
 * @brief The RolisteamImageProvider class is providing images to qml engine for charactersheet.
 */
class RolisteamImageProvider : public QQuickImageProvider
{
public:
    RolisteamImageProvider(charactersheet::ImageModel* model);
    virtual ~RolisteamImageProvider();
    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);

private:
    charactersheet::ImageModel* m_model;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
