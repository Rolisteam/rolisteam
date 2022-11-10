#include <QApplication>

#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QUndoStack>
#include <QVariant>

#include "data/campaignmanager.h"
#include "rwidgets/mediacontainers/pdfviewer.h"
#include "utils/iohelper.h"
#include <common_qml/theme.h>

#include "controller/view_controller/pdfcontroller.h"
#include "model/playermodel.h"
//#include "updater/media/pdfupdater.h"

/*void registerMindmapType()
{
    customization::Theme::setPath("/home/renaud/application/mine/renaudg/rolisteam/resources/stylesheet/qml/theme.ini");
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");

    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject*
                                                   {
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
                                                       "MindItemModel can't be created in qml");
}*/

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/rolistheme");

    PlayerModel* pmodel= new PlayerModel();
    // auto model= new RemotePlayerModel(pmodel);

    campaign::CampaignManager campaignManager(nullptr);
    campaignManager.openCampaign(
        QUrl("file:///home/renaud/application/mine/renaudg/rolisteam/src/tests/manual/pdf/campaign"));

    QUndoStack undoStack;
    // MindMapController::setRemotePlayerModel(model);

    PdfController ctrl(
        "uuid1",
        QUrl::fromUserInput(
            "/home/renaud/application/mine/renaudg/rolisteam/src/tests/manual/pdf/campaign/media/example.pdf"));
    // IOHelper::readPdfController(&ctrl, serializedData);
    // updater.addMediaController(&ctrl);
    QObject::connect(&ctrl, &PdfController::performCommand, &ctrl,
                     [&undoStack](QUndoCommand* cmd) { undoStack.push(cmd); });
    ctrl.setLocalGM(true);

    ctrl.setLocalId("uuid");
    ctrl.setOwnerId("uuid");

    PdfViewer view(&ctrl);

    view.show();
    return app.exec();
}
