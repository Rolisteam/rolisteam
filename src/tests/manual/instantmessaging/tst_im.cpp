#include <QApplication>

#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QUndoStack>
#include <QVariant>

#include "controller/instantmessagingcontroller.h"
#include "rwidgets/mediacontainers/instantmessagingview.h"

#include "data/player.h"
#include "model/playermodel.h"
#include <common_qml/theme.h>

void registerTypeTest()
{
    qDebug() << "registerType 1";
    customization::Theme::setPath("/home/renaud/application/mine/renaudg/rolisteam/resources/stylesheet/qml/theme.ini");
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");

    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject* {
                                                       auto instead= customization::Theme::instance();
                                                       engine->setObjectOwnership(instead, QQmlEngine::CppOwnership);
                                                       return instead;
                                                   });
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(viewsqml);
    // Q_INIT_RESOURCE(charactersheet);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    registerTypeTest();
    PlayerModel model;
    InstantMessagingController ctrl(&model);

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");

    Player p1("Obi", Qt::blue, true);
    model.addPlayer(&p1);

    Player p2("Player", Qt::green, false);
    model.addPlayer(&p2);

    InstantMessagingView view(&ctrl);

    view.show();

    return app.exec();
}
