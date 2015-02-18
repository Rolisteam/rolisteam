
#include <QCoreApplication>
#include "servermanager.h"
int main(int arg, char** args)
{
    QCoreApplication app(arg,args);
    app.setApplicationName("Rolisteam");
    app.setApplicationVersion("2.0.0");

    /** @todo read configuration or arguments */

    ServerManager* tmp = new ServerManager(6660,&app);
    tmp->start();

    return app.exec();

}
