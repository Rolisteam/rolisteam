#include <QCoreApplication>
#include <QTimer>
#include "readersender.h"

int main(int argc, char** argv)
{

    QCoreApplication app(argc,argv);

    ReaderSender reader;
    QObject::connect(&reader, SIGNAL(finish()), &app, SLOT(quit()), Qt::QueuedConnection);
    QTimer::singleShot(1000, &reader, SLOT(readFile()));

    return app.exec();

}
