#ifndef THEMELISTMODEL_H
#define THEMELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "theme.h"

class ThemeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ThemeListModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role);


    Theme getTheme(int row);
    /**
     * Load informations from the previous rolisteam's execution
     */
    void readSettings();

    /**
     * Save parameters for next executions.
     */
    void writeSettings();
public slots:
    void removeSelectedTheme(int row);
    void addTheme(Theme tmp);
private:
        ThemeList* m_themeList;
};

#endif // THEMELISTMODEL_H
