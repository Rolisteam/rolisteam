#ifndef USERLISTVIEW_H
#define USERLISTVIEW_H

#include <QTreeView>
class UserListModel;
class UserListDelegate;
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

private:
    UserListDelegate* m_delegate;
};

#endif // USERLISTVIEW_H
