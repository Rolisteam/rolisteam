#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>

class ImageModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Headers {Thumbnails,Key,Filename,User};
    explicit ImageModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QList<QPixmap>* imageData() const;
    void setImageData(QList<QPixmap>* imageData);

    void insertImage(QPixmap*,QString,QString);

    void clear();
private:
    QList<QPixmap>* m_imageData;
    QStringList m_keyList;
    QStringList m_filename;
    QStringList m_column;
};

#endif // IMAGEMODEL_H
