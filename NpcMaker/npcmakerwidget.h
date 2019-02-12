#ifndef NPCMAKERWIDGET_H
#define NPCMAKERWIDGET_H

#include "widgets/gmtoolbox/gamemastertool.h"
#include <QWidget>

namespace Ui
{
    class NpcMakerWidget;
}
class Character;
class GenericModel;
class NpcMakerWidget : public QWidget, public GameMasterTool
{
    Q_OBJECT

public:
    explicit NpcMakerWidget(QWidget* parent= nullptr);
    ~NpcMakerWidget();

    void setCharacter(Character* character);

protected slots:
    void importNpc();
    void exportNpc();

    void updateImage();

private:
    Ui::NpcMakerWidget* ui;
    GenericModel* m_actionModel= nullptr;
    GenericModel* m_shapeModel= nullptr;
    GenericModel* m_propertyModel= nullptr;
};

#endif // NPCMAKERWIDGET_H
