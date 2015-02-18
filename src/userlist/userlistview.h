#ifndef USERLISTVIEW_H
#define USERLISTVIEW_H

#include <QTreeView>
class UserListModel;
class UserListView : public QTreeView
{
    Q_OBJECT
public:
    explicit UserListView(QWidget *parent = 0);

signals:
    void currentItemChanged(const QModelIndex &);

public slots:


protected slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

};

#endif // USERLISTVIEW_H
