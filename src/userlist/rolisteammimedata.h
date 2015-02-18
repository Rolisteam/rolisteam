#ifndef ROLISTEAMMIMEDATA_H
#define ROLISTEAMMIMEDATA_H

#include <QMimeData>


#include "person.h"
class RolisteamMimeData : public QMimeData
{
    Q_OBJECT
public:
    RolisteamMimeData();
    bool hasFormat ( const QString & mimeType ) const;
    //QStringList QMimeData::formats () const;

    void setPerson(Person*);
    bool hasPerson() const;
    const Person* getData() const;
/*protected:
    QVariant retrieveData ( const QString & mimeType, QVariant::Type type ) const;*/
private:
    QString m_format;
    Person* m_data;
};
#endif // ROLISTEAMMIMEDATA_H
