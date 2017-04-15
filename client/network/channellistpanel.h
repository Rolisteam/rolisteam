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
    explicit ChannelListPanel(QWidget *parent = 0);
    virtual ~ChannelListPanel();

    void processMessage(NetworkMessageReader* msg);
    void sendOffModel();

private:
    Ui::ChannelListPanel *ui;
    ChannelModel* m_model;
};

#endif // CHANNELLISTPANEL_H
