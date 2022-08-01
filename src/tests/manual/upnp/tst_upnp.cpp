#include "network/upnp/upnpnat.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char* argv[])
{

    QCoreApplication app(argc, argv);
    UpnpNat nat;

    /*QFile file("/home/renaud/gatedesc0b.xml");
    if(!file.open(QIODevice::ReadOnly))
        qDebug() << "error can't read the file";

    auto all= QString(file.readAll());

    nat.setDescription(all);

    nat.new_parser_description();
    nat.parser_description();*/

    nat.init(5, 10);
    QObject::connect(&nat, &UpnpNat::discoveryEnd, [&nat](bool b) {
        if(b)
            nat.addPortMapping("upnpRolisteam", nat.localIp(), 6664, 6664, "TCP");
        // qDebug() << "Discovery END:"<<b;
    });
    QObject::connect(&nat, &UpnpNat::statusChanged, [&nat, &app]() {
        if(nat.status() == UpnpNat::NAT_STAT::NAT_ADD)
        {
            qDebug() << "It worked!";
            app.quit();
        }
    });

    QObject::connect(&nat, &UpnpNat::lastErrorChanged, [&nat]() { qDebug() << " Error:" << nat.lastError(); });

    nat.discovery();

    return app.exec();
}
