#ifndef CHANNELLISTPANEL_H
#define CHANNELLISTPANEL_H

#include <QWidget>

namespace Ui
{
    class ChannelListPanel;
}
#include "network/channelmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

class ChannelListPanel : public QWidget
{
    Q_OBJECT

public:
    enum Group
    {
        VIEWER= 0x0,
        GAMEMASTER= 0x1,
        ADMIN= 0x2
    };
    Q_DECLARE_FLAGS(Groups, Group)

    explicit ChannelListPanel(QWidget* parent= nullptr);
    virtual ~ChannelListPanel();

    void processMessage(NetworkMessageReader* msg);
    void sendOffModel();

    ChannelListPanel::Groups currentGroups() const;
    void setCurrentGroups(const Groups& currentGroups);

    bool isAdmin();
    template <typename T>
    T indexToPointer(QModelIndex index);

    QString serverName() const;
    void setServerName(const QString& serverName);

    void setLocalPlayerId(const QString& id);

    void cleanUp();

signals:
    void CurrentChannelGmIdChanged(QString gm);

public slots:
    void showCustomMenu(QPoint pos);
    void sendOffLoginAdmin(QByteArray str= QByteArray());

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

private:
    Ui::ChannelListPanel* ui;
    ChannelModel* m_model= nullptr;

    QAction* m_edit= nullptr;
    QAction* m_lock= nullptr;
    QAction* m_join= nullptr;
    QAction* m_channelPassword= nullptr;
    QAction* m_addChannel= nullptr;
    QAction* m_addSubchannel= nullptr;
    QAction* m_deleteChannel= nullptr;
    QAction* m_setDefault= nullptr;
    QAction* m_admin= nullptr;
    QAction* m_kick= nullptr;

    Groups m_currentGroups;
    QModelIndex m_index;
    QString m_serverName;
    QString m_localPlayerId;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelListPanel::Groups)

#endif // CHANNELLISTPANEL_H
