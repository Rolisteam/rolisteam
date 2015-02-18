#ifndef USERLISTWIDGET_H
#define USERLISTWIDGET_H

#include <QDockWidget>
#include <QModelIndex>

class QPushButton;
class QSlider;
class QLCDNumber;
class UserListView;
class UserListModel;
class QVBoxLayout;
class QHBoxLayout;
class Person;
class Player;
class UserListWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit UserListWidget(QWidget *parent = 0);
    ~UserListWidget();

    QList<Person*>* getSelectedPerson();
protected:
    void setupUI();
    void setAction();
signals:
    void sizePcChanged(int);
    void opentchat();

public slots:
    void addUser(Person* p);
    void setLocalPlayer(Player* p);


protected slots:
    void addPC();
    void delSelectedPC();
    void currentChanged(const QModelIndex&);


private:
    QPushButton* m_tchatButton;
    QPushButton* m_addPC;
    QPushButton* m_delPC;
    QSlider* m_sizePC;
    QLCDNumber* m_sizePCNumber;
    UserListView* m_view;
    UserListModel* m_model;

    QVBoxLayout* m_verticalLayout;
    QHBoxLayout* m_horizontalLayoutSlider;
    QHBoxLayout* m_horizontalLayoutButton;
    QWidget* m_centralWidget;
    Player* m_local;
    Player* m_gameMaster;
};

#endif // USERLISTWIDGET_H
