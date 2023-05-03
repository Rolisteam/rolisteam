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
#include "test_root_path.h"

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
        QUrl(QString("file://%1/manual/pdf/campaign").arg(tests::root_path)));

    QUndoStack undoStack;

    PdfController ctrl(
        "uuid1",
        QUrl::fromUserInput(QString("file://%1/manual/pdf/campaign/media/example.pdf").arg(tests::root_path)));
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
