#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>

#include "rwidgets/gmtoolbox/NameGenerator/namegeneratorwidget.h"
#include "rwidgets/gmtoolbox/UnitConvertor/convertor.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QMainWindow main;

    QWidget* wid= new QWidget(&main);
    QHBoxLayout* layout= new QHBoxLayout;

    layout->addWidget(new NameGeneratorWidget());
    layout->addWidget(new GMTOOL::Convertor());

    wid->setLayout(layout);

    main.setCentralWidget(wid);

    main.show();

    return app.exec();
}
