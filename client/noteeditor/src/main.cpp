#include "textedit.h"
#include <QApplication>



#if defined _COMPOSE_STATIC_
#include <QtPlugin>
#if defined _USE_qjpeg
Q_IMPORT_PLUGIN(qjpeg)
#endif
#if defined _USE_qmng
Q_IMPORT_PLUGIN(qmng)
#endif
#if defined _USE_qgif
Q_IMPORT_PLUGIN(qgif)
#endif
#if defined _USE_qtiff
Q_IMPORT_PLUGIN(qtiff)
#endif
#endif





int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    a.setOrganizationName("Oasis Test Reader");
    a.setOrganizationDomain("QTuser");
    a.setApplicationName("Mini Office");
    #if QT_VERSION >= 0x040500
    qDebug() << "### QT_VERSION main  -> " << QT_VERSION;
    qDebug() << "### QT_VERSION_STR main -> " << QT_VERSION_STR;
    #endif
    TextEdit mw;
    mw.resize( 700, 450 );
    mw.show();
    QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
