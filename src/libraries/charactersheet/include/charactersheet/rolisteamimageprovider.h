#ifndef ROLISTEAMIMAGEPROVIDER_H
#define ROLISTEAMIMAGEPROVIDER_H

#include "imagemodel.h"
#include <QQuickImageProvider>
#include <QSharedPointer>

#include <charactersheet/charactersheet_global.h>

/**
 * @brief The RolisteamImageProvider class is providing images to qml engine for charactersheet.
 */
class CHARACTERSHEET_EXPORT RolisteamImageProvider : public QQuickImageProvider
{
    Q_OBJECT
public:
    RolisteamImageProvider(charactersheet::ImageModel* model);
    virtual ~RolisteamImageProvider();
    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);

private:
    QPointer<charactersheet::ImageModel> m_model;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
