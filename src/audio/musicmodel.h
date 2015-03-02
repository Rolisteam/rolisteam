#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include <QAbstractTableModel>

class MusicModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MusicModel(QObject *parent = 0);

signals:

public slots:

};

#endif // MUSICMODEL_H
