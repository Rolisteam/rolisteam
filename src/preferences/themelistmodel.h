#ifndef THEMELISTMODEL_H
#define THEMELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
class Theme;
class ThemeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ThemeListModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role);
public slots:
    void removeSelectedTheme();
    void addTheme();
private:
        QList<Theme*>* m_themeList;
};

#endif // THEMELISTMODEL_H
