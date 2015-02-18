#ifndef ROLISTEAMMIMEDATA_H
#define ROLISTEAMMIMEDATA_H

#include <QMimeData>


#include "person.h"
/**
 * @brief allows drag and drop inside the application scope.
 */
class RolisteamMimeData : public QMimeData
{
    Q_OBJECT
public:
    /**
      *  @brief default constructor
      */
    RolisteamMimeData();
    /**
      *  @brief return true when given parameter describe a format is stored into the class
      */
    bool hasFormat ( const QString & mimeType ) const;
    //QStringList QMimeData::formats () const;

    /**
      *  @brief to define the draged and droped person.
      */
    void setPerson(Person*);
    /**
      *  @brief same utility of hasformat except is dedicated to person instance.
      */
    bool hasPerson() const;
    /**
      *  @brief return the person instance.
      */
    const Person* getData() const;
/*protected:
    QVariant retrieveData ( const QString & mimeType, QVariant::Type type ) const;*/
private:
    QString m_format; /// const string representing the minetype of person
    Person* m_data; /// pointer to stored data
};
#endif // ROLISTEAMMIMEDATA_H
