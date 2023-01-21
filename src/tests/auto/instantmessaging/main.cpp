#include "instantmessaging.h"

#include "common_qml/theme.h"
#include "data/player.h"
#include "message.h"
#include "model/playermodel.h"
#include "qml_components/avatarprovider.h"
#include "rwidgets/mediacontainers/instantmessagingview.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(viewsqml);
    customization::Theme::setPath(":/resources/stylesheet/qml/theme.ini");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");
    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject* {
                                                       auto instead= customization::Theme::instance();
                                                       engine->setObjectOwnership(instead, QQmlEngine::CppOwnership);
                                                       return instead;
                                                   });

    qmlRegisterSingletonType<InstantMessagerManager>("org.rolisteam.InstantMessaging", 1, 0, "InstantMessagerManager",
                                                     [](QQmlEngine* qmlengine, QJSEngine* scriptEngine) -> QObject* {
                                                         Q_UNUSED(scriptEngine)
                                                         static InstantMessagerManager manager;
                                                         qmlengine->setObjectOwnership(&manager,
                                                                                       QQmlEngine::CppOwnership);
                                                         static bool initialized= false;
                                                         if(!initialized)
                                                         {
                                                             manager.setCtrl(nullptr);
                                                             initialized= true;
                                                         }
                                                         return &manager;
                                                     });

    auto engine= new QQmlApplicationEngine();
    auto player= new PlayerModel();
    auto avatar= new AvatarProvider(player);
    auto ap= new Player("aaa", "Obi", QColor(Qt::red));
    player->addPlayer(ap);
    engine->addImageProvider("avatar", avatar);
    engine->addImportPath("qrc:/qml");
    auto r= new Refresh;

    QObject::connect(r, &Refresh::refreshChanged, &a, [=]() {
        engine->load(QUrl(""));
        engine->load(QUrl("qrc:/main.qml"));
    });
    engine->rootContext()->setContextProperty("refresher", r);
    engine->rootContext()->setContextProperty("model", new Model);
    engine->load(QUrl("qrc:/main.qml"));
    return a.exec();
}
