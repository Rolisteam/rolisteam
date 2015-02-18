#ifndef USERLISTMODEL_H
#define USERLISTMODEL_H

#include <QAbstractItemModel>
#include "person.h"

class PersonItem
{
public:
    PersonItem(Person* p,bool isLeaf);
    void setPerson(Person* p);
    Person* getPerson();

    bool isLeaf();
    void setLeaf(bool leaf);

    PersonItem* getParent();
    void setParent(PersonItem* p);

    void addChild(PersonItem* child);
    PersonItem* child(int row);

    int row();
    int indexOfChild(PersonItem* itm);
    int indexOfPerson(Person* itm);

    int childrenCount();
private:
    Person* m_data;
    bool m_isLeaf;
    PersonItem* m_parent;
    QList<PersonItem*>* m_children;
};

class UserListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit UserListModel(QObject *parent = 0);


    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    virtual void addPlayer(Person* p);
    bool isPlayer(const QModelIndex& );
    virtual void addCharacter(Person* p,Person* parent);
    Qt::ItemFlags flags ( const QModelIndex & index )  const;
    virtual void setLocalPlayer(Person* p);
signals:

public slots:

private:
    PersonItem* m_root;
    Person* m_player;
};

#endif // USERLISTMODEL_H
