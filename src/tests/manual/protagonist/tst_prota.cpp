#include <QApplication>

#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QUndoStack>
#include <QVariant>

#include "data/campaignmanager.h"
#include "rwidgets/docks/antagonistboard.h"
#include <common_qml/theme.h>

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

    campaign::CampaignManager campaignManager(nullptr);
    campaignManager.openCampaign(
        QUrl("file:///home/renaud/application/mine/renaudg/rolisteam/src/tests/manual/protagonist/campaign"));

    QUndoStack undoStack;

    campaign::AntagonistBoard board(campaignManager.editor());
    QObject::connect(&app, &QApplication::aboutToQuit, [&campaignManager]() { campaignManager.saveCampaign(); });

    board.show();
    return app.exec();
}
