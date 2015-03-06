#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include <QAbstractTableModel>
#include <QMediaPlayer>

class MusicModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum COLUMN { TITLE };
    /**
     * @brief MusicModel
     * @param parent
     */
    explicit MusicModel(QObject *parent = 0);
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    virtual int columnCount(const QModelIndex & parent= QModelIndex()) const;
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    /**
     * @brief addSong
     */
    void addSong(QStringList );
    /**
     * @brief getMediaByModelIndex
     * @return
     */
    QMediaContent* getMediaByModelIndex(QModelIndex);

    void removeAll();
signals:

public slots:

private:
    QStringList m_header;
    QList<QMediaContent*> m_data;
    QMediaPlayer* m_player;
};

#endif // MUSICMODEL_H
