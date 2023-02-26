#include <QApplication>

#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QUndoStack>
#include <QVariant>
#include <QIcon>

#include "rwidgets/dialogs/imageselectordialog.h"
#include "controller/view_controller/imageselectorcontroller.h"
//#include ""

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);
#ifdef Q_OS_WIN
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << ":/resources");
    QIcon::setThemeName("rolistheme");
#endif

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/rolistheme");

    auto ctrl = new ImageSelectorController(true, ImageSelectorController::All, ImageSelectorController::Square);
    ImageSelectorDialog dialog(ctrl);


    QObject::connect(&dialog, &ImageSelectorDialog::accepted, [ctrl](){
        ctrl->finalImageData();
    });


    dialog.show();


    return app.exec();
}
