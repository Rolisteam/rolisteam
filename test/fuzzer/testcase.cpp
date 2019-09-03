#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include "formula/formulamanager.h"
using namespace Formula;
static FormulaManager* parser= new FormulaManager();


void runCommand(const QString& cmd)
{
//    qDebug() << "cmd" << cmd;
    parser->getValue(cmd);
}


int main(int argc, char** argv)
{
    // qDebug() << "first";
    QCoreApplication app(argc, argv);

    // qDebug() << "start";
    QFile file(app.arguments().at(1));
    // qDebug() << "file" << app.arguments().at(1);
    if(!file.open(QIODevice::ReadOnly))
        return 1;

    auto line= file.readLine();
    while(!line.isEmpty())
    {
        // qDebug() << line;
        runCommand(QString::fromUtf8(line));
        line= file.readLine();
    }

    return 0;
}
