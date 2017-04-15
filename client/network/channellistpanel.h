#ifndef CHANNELLISTPANEL_H
#define CHANNELLISTPANEL_H

#include <QWidget>

namespace Ui {
class ChannelListPanel;
}

class ChannelListPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelListPanel(QWidget *parent = 0);
    ~ChannelListPanel();

private:
    Ui::ChannelListPanel *ui;
};

#endif // CHANNELLISTPANEL_H
