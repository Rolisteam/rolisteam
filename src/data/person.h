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
    virtual QString getName() const;
    virtual QColor getColor() const;
    virtual void setName(QString);
    virtual void setColor(QColor);

    virtual void setState(Qt::CheckState m);
    virtual Qt::CheckState checkedState();

protected:
    QString m_name;
    QColor m_color;
    Qt::CheckState m_state;
};

#endif // PERSON_H
