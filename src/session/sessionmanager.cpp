#include "sessionmanager.h"
#include <QTreeView>
#include <QHBoxLayout>
#include "sessionitemmodel.h"

SessionManager::SessionManager()
{
    m_view =new QTreeView;
    m_layout = new QHBoxLayout;
    m_layout->addWidget(m_view);
    m_internal = new QWidget();
    m_internal->setLayout(m_layout);
    setWidget(m_internal);

    setWindowTitle(tr("Resources Explorer"));


    m_model = new SessionItemModel;
    m_view->setModel(m_model);
}
