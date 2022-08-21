#include <QApplication>

#include "controller/playercontroller.h"
#include "docks/playerspanel.h"
#include "model/playermodel.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    PlayerController ctrl;

    PlayerModel* model= ctrl.model();

    model->addPlayer(new Player("GM", Qt::blue, true));

    auto p= new Player("Player1", Qt::green, false);
    p->addCharacter("Paul Atreide", Qt::magenta, {}, {}, false);
    model->addPlayer(p);
    auto p1= new Player("Player2", Qt::red, false);
    p1->addCharacter("Baron Arkonen", Qt::cyan, {}, {}, false);
    model->addPlayer(p1);

    PlayersPanel view(&ctrl);

    view.show();
    return app.exec();
}
