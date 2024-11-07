#ifndef INTEGRITYCONTROLLER_H
#define INTEGRITYCONTROLLER_H

#include <QObject>
#include <QPointer>

#include "charactersheet/charactersheetmodel.h"
#include "fieldmodel.h"

class IntegrityController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(FieldModel* field READ field WRITE setField NOTIFY fieldChanged FINAL)
    Q_PROPERTY(CharacterSheetModel* sheets READ sheets WRITE setSheets NOTIFY sheetsChanged FINAL)
public:
    explicit IntegrityController(QObject* parent= nullptr);

    FieldModel* field() const;
    void setField(FieldModel* newField);

    CharacterSheetModel* sheets() const;
    void setSheets(CharacterSheetModel* newSheets);

    void fieldRemoved(const QString& id);
    void fieldAdded(CSItem* item);
    void checkAll();

    void checkIntegrity(CharacterSheet* sheet);
    void checkField();

signals:
    void fieldChanged();
    void sheetsChanged();

private:
    QPointer<FieldModel> m_field;
    QPointer<CharacterSheetModel> m_sheets;
};

#endif // INTEGRITYCONTROLLER_H
