#include <QCoreApplication>
#include "readersender.h"

int main(int argc, char** argv)
{

    QCoreApplication app(argc,argv);

    ReaderSender reader;
    reader.readFile();


    return app.exec();

}
