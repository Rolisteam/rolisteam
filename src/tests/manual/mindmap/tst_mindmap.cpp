#include <QApplication>

#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QUndoStack>
#include <QVariant>

#include "controller/view_controller/mindmapcontroller.h"
#include "data/campaignmanager.h"
#include "rwidgets/mediacontainers/mindmapview.h"
#include "utils/iohelper.h"
#include "utils/mappinghelper.h"
#include <common_qml/theme.h>

#include "controller/view_controller/sidemenucontroller.h"
#include "model/playermodel.h"
// #include "utils/iohelper.h"
#include "mindmap/data/nodestyle.h"
#include "mindmap/qmlItems/linkitem.h"
#include "test_root_path.h"
#include "updater/media/mindmapupdater.h"
#include "worker/iohelper.h"

void registerMindmapType()
{
    customization::Theme::setPath(QString("%1/../../resources/stylesheet/qml/theme.ini").arg(tests::root_path));
    /*qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");

    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject* {
                                                       auto instead= customization::Theme::instance();
                                                       engine->setObjectOwnership(instead, QQmlEngine::CppOwnership);
                                                       return instead;
                                                   });

    qmlRegisterType<utils::MappingHelper>("utils", 1, 0, "MappingHelper");
    qmlRegisterUncreatableType<MindMapController>("mindmap", 1, 0, "MindMapController",
                                                  "MindMapController can't be created in qml");
    qmlRegisterUncreatableType<mindmap::MindItem>("mindmap", 1, 0, "MindItem", "Enum only");
    qmlRegisterType<mindmap::SelectionController>("mindmap", 1, 0, "SelectionController");
    qmlRegisterUncreatableType<RemotePlayerModel>("mindmap", 1, 0, "RemotePlayerModel", "property values");
    qmlRegisterType<mindmap::LinkItem>("mindmap", 1, 0, "MindLink");
    qmlRegisterType<mindmap::NodeStyle>("mindmap", 1, 0, "NodeStyle");
    qmlRegisterUncreatableType<mindmap::PositionedItem>("mindmap", 1, 0, "PositionedItem", "Enum only");
    qmlRegisterType<mindmap::SideMenuController>("mindmap", 1, 0, "SideMenuController");
    qmlRegisterUncreatableType<mindmap::MindItemModel>("mindmap", 1, 0, "MindItemModel",
                                                       "MindItemModel can't be created in qml");*/

    customization::Theme::setPath(":/resources/stylesheet/qml/theme.ini");
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterUncreatableType<mindmap::MindMapControllerBase>("mindmapcpp", 1, 0, "MindMapController",
                                                               "MindMapController can't be created in qml");
    qmlRegisterUncreatableType<mindmap::MindItem>("mindmapcpp", 1, 0, "MindItem", "Enum only");
    qmlRegisterUncreatableType<RemotePlayerModel>("mindmapcpp", 1, 0, "RemotePlayerModel", "property values");
    qmlRegisterType<mindmap::LinkItem>("mindmapcpp", 1, 0, "MindLink");
    qmlRegisterType<mindmap::NodeStyle>("mindmapcpp", 1, 0, "NodeStyle");
    qmlRegisterType<mindmap::MindNode>("mindmapcpp", 1, 0, "MindNode");
    qmlRegisterType<mindmap::SideMenuController>("mindmapcpp", 1, 0, "SideMenuController");
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    auto format= QSurfaceFormat::defaultFormat();
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");

    PlayerModel* pmodel= new PlayerModel();
    registerMindmapType();
    auto model= new RemotePlayerModel(pmodel);

    campaign::CampaignManager campaignManager(nullptr);
    campaignManager.openCampaign(QUrl(QString("file://%1/manual/mindmap/campaign").arg(tests::root_path)));

    MindMapUpdater updater(nullptr, &campaignManager);
    auto serializedData
        = utils::IOHelper::loadFile(QString("file://%1/manual/mindmap/campaign/media/test.rmap").arg(tests::root_path));

    QUndoStack undoStack;
    MindMapController::setRemotePlayerModel(model);

    MindMapController ctrl("uuid1");
    IOHelper::readMindmapController(&ctrl, serializedData);
    updater.addMediaController(&ctrl);
    QObject::connect(&ctrl, &MindMapController::performCommand, &ctrl,
                     [&undoStack](QUndoCommand* cmd) { undoStack.push(cmd); });
    ctrl.setLocalGM(true);

    ctrl.setLocalId("uuid");
    ctrl.setOwnerId("uuid");

    MindMapView view(&ctrl);

    view.show();
    return app.exec();
}
