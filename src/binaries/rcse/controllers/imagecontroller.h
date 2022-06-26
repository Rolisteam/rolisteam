#ifndef IMAGECONTROLLER_H
#define IMAGECONTROLLER_H

#include "charactersheet/imagemodel.h"
#include <QObject>
#include <QString>
#include <memory>

class QTableView;
class QAction;
class RolisteamImageProvider;
class ImageController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(charactersheet::ImageModel* model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
public:
    ImageController(QObject* parent= nullptr);

    void clearData();

    QString uuid() const;
    charactersheet::ImageModel* model() const;
    QSize backgroundSize() const;

    RolisteamImageProvider* provider() const;

public slots:
    void addBackgroundImage(int idx, const QPixmap& pix, const QString& filename, const QString& uuid);
    void addImage(const QPixmap& pix, const QString& filename);
    void removeImage(int idx);
    void setUuid(const QString& uuid);
    void copyPath(const QModelIndex& index);
    void copyUrl(const QModelIndex& index);
    void replaceImage(const QModelIndex& index, const QString& filepath);
    void reloadImage(const QModelIndex& index);

signals:
    void dataChanged();
    void uuidChanged();
    void modelChanged();
    void errorOccurs(QString);

private:
    std::unique_ptr<charactersheet::ImageModel> m_model;
    QString m_uuid;
};

#endif
