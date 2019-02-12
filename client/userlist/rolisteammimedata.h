#ifndef ROLISTEAMMIMEDATA_H
#define ROLISTEAMMIMEDATA_H

#include <QMimeData>

#include "data/diceshortcut.h"
#include "data/person.h"
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
    bool hasFormat(const QString& mimeType) const;
    // QStringList QMimeData::formats () const;

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
    Person* getData() const;

    /**
     * @brief getAlias
     * @return
     */
    DiceShortCut getAlias() const;
    void setAlias(QString, QString, bool);

private:
    QString m_format;     /// const string representing the minetype of person
    Person* m_data;       /// pointer to stored data
    DiceShortCut m_alias; /// alias
};
#endif // ROLISTEAMMIMEDATA_H
