#ifndef CHANNELLISTPANEL_H
#define CHANNELLISTPANEL_H

#include <QWidget>
#include <memory>

#include "controller/networkcontroller.h"
#include "network/channelmodel.h"
#include "network/networkmessagereader.h"
#include "preferences/preferencesmanager.h"
#include "rwidgets_global.h"

namespace Ui
{
class ChannelListPanel;
}

class RWIDGET_EXPORT ChannelListPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelListPanel(PreferencesManager* preferences, NetworkController* ctrl, QWidget* parent= nullptr);
    virtual ~ChannelListPanel();

    template <typename T>
    T indexToPointer(QModelIndex index);

    QString serverName() const;
    void setServerName(const QString& serverName);

    void setLocalPlayerId(const QString& id);

    void cleanUp();

    ServerConnection* getClient(QModelIndex index);
    Channel* getChannel(QModelIndex index);
signals:
    void CurrentChannelGmIdChanged(QString gm);

public slots:
    void showCustomMenu(QPoint pos);

protected:
    void moveUserToCurrent();
protected slots:
    void addChannelAsSibbling();
    void lockChannel();
    void kickUser();
    void banUser();
    void addChannel();
    void setPasswordOnChannel();
    void deleteChannel();
    void joinChannel();
    void editChannel();
    void logAsAdmin();
    void resetChannel();

private:
    Ui::ChannelListPanel* ui;
    QPointer<NetworkController> m_ctrl;
    QPointer<PreferencesManager> m_pref;

    QAction* m_edit= nullptr;
    QAction* m_lock= nullptr;
    QAction* m_join= nullptr;
    QAction* m_ban= nullptr;
    QAction* m_channelPassword= nullptr;
    QAction* m_addChannel= nullptr;
    QAction* m_addSubchannel= nullptr;
    QAction* m_deleteChannel= nullptr;
    QAction* m_setDefault= nullptr;
    QAction* m_admin= nullptr;
    QAction* m_kick= nullptr;
    QAction* m_resetChannel= nullptr;
    QAction* m_moveUserToCurrentChannel= nullptr;

    QModelIndex m_index;
    QString m_serverName;
    QString m_localPlayerId;
};

#endif // CHANNELLISTPANEL_H
