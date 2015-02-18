#ifndef PERSON_H
#define PERSON_H
#include <QString>
#include <QColor>

class Person
{
public:
    Person(QString name,QColor color);
    Person();

    virtual bool hasChildren() const =0;
    virtual QString getName() const ;
    virtual QColor getColor() const;
    virtual void setName(QString);
    virtual void setColor(QColor);

protected:
    QString m_name;
    QColor m_color;
};

#endif // PERSON_H
