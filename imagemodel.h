#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>
#include <QJsonArray>
#include <QJsonObject>

#include "charactersheet/rolisteamimageprovider.h"


class ImageModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Headers {Key,Filename,Background};
    explicit ImageModel(QHash<QString,QPixmap*>& list,QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QList<QPixmap>* imageData() const;
    void setImageData(QList<QPixmap>* imageData);

    void insertImage(QPixmap*, QString, QString, bool isBg);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void clear();

    QJsonArray save();

    void removeImageAt(const QModelIndex& index);

    void setImageProvider(RolisteamImageProvider* img);
    void setPixList(QHash<QString,QPixmap*>& list);


    bool isBackgroundById(QString id);

    //QHash<>getPixHash() const;
private:
    QList<QPixmap>* m_imageData;
    QStringList m_keyList;
    QStringList m_filename;
    std::vector<bool> m_background;
    QStringList m_column;
    QHash<QString,QPixmap*>& m_list;
    RolisteamImageProvider* m_provider;
};

#endif // IMAGEMODEL_H
