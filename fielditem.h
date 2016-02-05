#ifndef FIELDITEM_H
#define FIELDITEM_H

#include <QQmlComponent>

class FieldItem : public QQmlComponent
{
public:
    FieldItem(QQmlEngine * engine, const QUrl & url, QObject * parent = 0);
};

#endif // FIELDITEM_H
