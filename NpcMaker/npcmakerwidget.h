#ifndef NPCMAKERWIDGET_H
#define NPCMAKERWIDGET_H

#include <QWidget>

namespace Ui {
class NpcMakerWidget;
}

class NpcMakerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NpcMakerWidget(QWidget *parent = nullptr);
    ~NpcMakerWidget();

private:
    Ui::NpcMakerWidget *ui;
};

#endif // NPCMAKERWIDGET_H
