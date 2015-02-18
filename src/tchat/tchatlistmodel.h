#ifndef TCHATLISTMODEL_H
#define TCHATLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

class Person;

class TchatListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TchatListModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setClients(QList<Person*>* tmp);
signals:


public slots:



private:
    QList<Person*>* m_dataList;

};

#endif // TCHATLISTMODEL_H
