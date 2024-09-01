#include <QApplication>

#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QUndoStack>
#include <QVariant>

#include "controller/view_controller/charactersheetcontroller.h"
#include "data/campaignmanager.h"
#include "mindmap/include/mindmap/data/positioneditem.h"
#include "rwidgets/mediacontainers/charactersheetwindow.h"
#include "utils/iohelper.h"
#include "utils/mappinghelper.h"
#include <common_qml/theme.h>

#include "controller/view_controller/sidemenucontroller.h"
#include "mindmap/data/nodestyle.h"
#include "mindmap/qmlItems/linkitem.h"
#include "model/playermodel.h"
#include "worker/modelhelper.h"
#include "test_root_path.h"

void registerMindmapType()
{
    customization::Theme::setPath(QString("%1/../../resources/stylesheet/qml/theme.ini").arg(tests::root_path));
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");

    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject*
                                                   {
                                                       auto instance= customization::Theme::instance();
                                                       engine->setObjectOwnership(instance, QQmlEngine::CppOwnership);
                                                       return instance;
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
                                                       "MindItemModel can't be created in qml");
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");

    campaign::CampaignManager campaignManager(nullptr);
    campaignManager.openCampaign(
        QUrl(QString("file://%1/manual/charactersheet/campaign").arg(tests::root_path)));

    auto serializedData= utils::IOHelper::loadFile(QString("/%1/manual/charactersheet/campaign/media/tf2.rcs").arg(tests::root_path));
    Q_UNUSED(serializedData)
    QUndoStack undoStack;

    CharacterSheetUpdater updater(nullptr ,&campaignManager);

    CharacterSheetController ctrl;
    QUrl path(QString("file://%1/manual/charactersheet/campaign/media/tf2.rcs").arg(tests::root_path));
    ctrl.setUrl(path);

    auto model= ctrl.model();
    auto img= ctrl.imageModel();
    auto root= ctrl.rootJson();
    QString qml;
    ModelHelper::loadCharacterSheet(path.toLocalFile(), model, img, root, qml);
    ctrl.setQmlCode(qml);
    ctrl.setRootJson(root);

    //IOHelper::readCharacterSheetController(&ctrl, serializedData);

    qDebug() << "character model: " << model->rowCount();

    CharacterSheetWindow view(&ctrl);

    view.show();
    return app.exec();
}
