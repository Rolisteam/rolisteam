#ifndef CHANNELLISTPANEL_H
#define CHANNELLISTPANEL_H

#include <QWidget>

namespace Ui {
class ChannelListPanel;
}
#include "network/channelmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

class ChannelListPanel : public QWidget
{
    Q_OBJECT

public:
    enum GROUP {VIEWER,ADMIN};
    explicit ChannelListPanel(QWidget *parent = 0);
    virtual ~ChannelListPanel();

    void processMessage(NetworkMessageReader* msg);
    void sendOffModel();

    ChannelListPanel::GROUP currentGRoup() const;
    void setCurrentGRoup(const GROUP &currentGRoup);

    bool isAdmin();
    template <typename T>
    T indexToPointer(QModelIndex index);

    QString serverName() const;
    void setServerName(const QString &serverName);

    void setLocalPlayerId(const QString& id);

    void cleanUp();

public slots:
    void showCustomMenu(QPoint pos);
    void sendOffLoginAdmin(QByteArray str = QByteArray());

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
    Ui::ChannelListPanel *ui;
    ChannelModel* m_model = nullptr;

    QAction* m_edit = nullptr;
    QAction* m_lock = nullptr;
    QAction* m_join = nullptr;
    QAction* m_channelPassword = nullptr;
    QAction* m_addChannel = nullptr;
    QAction* m_addSubchannel = nullptr;
    QAction* m_deleteChannel = nullptr;
    QAction* m_setDefault = nullptr;
    QAction* m_admin = nullptr;
    QAction* m_kick = nullptr;


    GROUP m_currentGroup;
    QModelIndex m_index;
    QString m_serverName;
    QString m_localPlayerId;
};

#endif // CHANNELLISTPANEL_H
