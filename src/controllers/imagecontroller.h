#ifndef IMAGECONTROLLER_H
#define IMAGECONTROLLER_H

#include "imagemodel.h"
#include <QObject>
#include <QString>
#include <memory>

class ImageModel;
class QTableView;
class QAction;
class RolisteamImageProvider;
class ImageController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid NOTIFY uuidChanged)
    Q_PROPERTY(ImageModel* model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
public:
    ImageController(QTableView* view, QObject* parent= nullptr);

    void clearData();
    void save(QJsonObject& obj) const;
    void load(const QJsonObject& obj);

    QString uuid() const;
    ImageModel* model() const;
    QSize backgroundSize() const;

    RolisteamImageProvider* getNewProvider() const;

public slots:
    void addBackgroundImage(int idx, const QPixmap& pix, const QString& filename);
    void addImage(const QPixmap& pix, const QString& filename);
    void removeImage(int idx);
    void setUuid(const QString& uuid);
    void contextualMenu(const QPoint& pos);

signals:
    void dataChanged();
    void uuidChanged();
    void modelChanged();

    void errorOccurs(QString);

protected slots:
    void copyPath();
    void copyUrl();
    void replaceImage();
    void reloadImageFromFile();

private:
    QAction* m_copyPath= nullptr;
    QAction* m_copyUrl= nullptr;
    QAction* m_replaceImage= nullptr;
    QAction* m_removeImage= nullptr;
    QAction* m_reloadImageFromFile= nullptr;

    std::unique_ptr<ImageModel> m_model;
    QTableView* m_view= nullptr;
    QString m_uuid;
};

#endif
