#ifndef USERLISTDOCKWIDGET_H
#define USERLISTDOCKWIDGET_H


#include "ui_userlistdockwidget.h"

class UserListDockWidget : public QDockWidget
{
public:
    UserListDockWidget();


private:
    Ui::UiUserListDockWidget m_dock;
};

#endif // USERLISTDOCKWIDGET_H
