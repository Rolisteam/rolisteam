#include <QApplication>

#include <QVariant>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/character.h"
#include "media/mediatype.h"
#include "rwidgets/mediacontainers/vmapframe.h"
#include "utils/iohelper.h"
#include "worker/iohelper.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QList<CharacterState*> states;
    auto cs1= new CharacterState();
    cs1->setId("ii");
    cs1->setLabel("State 1");
    cs1->setColor(Qt::green);
    cs1->setImagePath(":/img/arbre_square_500.jpg");

    states << cs1;

    auto cs2= new CharacterState();
    cs2->setId("iO");
    cs2->setLabel("State 2");
    cs2->setColor(Qt::blue);
    cs2->setImagePath(":/img/lion.jpg");

    states << cs2;

    Character::setListOfCharacterState(&states);

    VectorialMapController ctrl;
    ctrl.setLocalGM(true);
    ctrl.setActive(true);
    ctrl.setLocalId("uuid");
    ctrl.setOwnerId("uuid");

    Character character("aaa", "Lynn Gray-Rike", QColor("#0000DD"), false, 0);
    character.setAvatar(utils::IOHelper::loadFile(
        "/home/renaud/documents/03_jdr/01_Scenariotheque/09_Cops/03_krom/lynn_gray_rike.jpg"));

    QList<CharacterAction*> acts;

    character.defineActionList(acts);
    QPointF pos{200, 200};

    std::map<QString, QVariant> params;
    params.insert({Core::vmapkeys::KEY_CHARACTER, QVariant::fromValue(&character)});
    params.insert({Core::vmapkeys::KEY_POS, pos});
    params.insert({Core::vmapkeys::KEY_TOOL, Core::SelectableTool::PlayableCharacter});

    auto c= new vmap::CharacterItemController(params, &ctrl);
    VMapFrame frame(&ctrl);

    frame.setVisible(true);
    ctrl.addRemoteItem(c);

    return app.exec();
}
