#include "fielditem.h"

FieldItem::FieldItem(QQmlEngine * engine, const QUrl & url, QObject * parent )
    : QQmlComponent(engine,url,parent)
{

}

