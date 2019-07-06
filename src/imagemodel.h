#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>

class ImageModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QSize backgroundSize READ backgroundSize NOTIFY backgroundSizeChanged)
public:
    enum Headers
    {
        Key,
        Filename,
        Background
    };
    struct ImageInfo
    {
        QPixmap pixmap;
        bool isBackground;
        QString filename;
        QString key;
    };
    explicit ImageModel(QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    bool insertImage(const QPixmap& pix, const QString& key, const QString& path, bool isBg);
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void clear();

    void save(QJsonArray& array) const;

    void removeImageAt(const QModelIndex& index);
    void setPathFor(const QModelIndex& index, const QString& path);
    void setPixList(QHash<QString, QPixmap*>& list);
    bool isBackgroundById(QString id) const;
    void removeImageByKey(const QString& key);

    QSize backgroundSize() const;
    void removeImage(int i);
    QPixmap pixmapFromKey(QString id);


signals:
    void backgroundSizeChanged();

private:
    QStringList m_column;
    std::vector<ImageInfo> m_data;
};

#endif // IMAGEMODEL_H
