#include "integritycontroller.h"

#include "charactersheet/controllers/tablefield.h"

template <typename T>
T* copyField(CSItem* item)
{
    auto field= new T(item->itemType(), false);
    field->copyField(item, true);
    return field;
}

IntegrityController::IntegrityController(QObject* parent) : QObject{parent} {}

FieldModel* IntegrityController::field() const
{
    return m_field;
}

void IntegrityController::setField(FieldModel* newField)
{
    if(m_field == newField)
        return;

    if(m_field)
        disconnect(m_field, nullptr, this, nullptr);

    m_field= newField;
    connect(m_field, &FieldModel::rowsInserted, this, &IntegrityController::checkAll);
    connect(m_field, &FieldModel::rowsRemoved, this, &IntegrityController::checkAll);
    connect(m_field, &FieldModel::dataChanged, this, &IntegrityController::checkField);
    connect(m_field, &FieldModel::valuesChanged, this, &IntegrityController::checkField);
    connect(m_field, &FieldModel::fieldAdded, this, &IntegrityController::checkAll);
    connect(m_field, &FieldModel::fieldRemoved, this, &IntegrityController::checkAll);
    emit fieldChanged();
}

CharacterSheetModel* IntegrityController::sheets() const
{
    return m_sheets;
}

void IntegrityController::setSheets(CharacterSheetModel* newSheets)
{
    if(m_sheets == newSheets)
        return;

    if(m_sheets)
        disconnect(m_sheets, nullptr, this, nullptr);

    m_sheets= newSheets;
    connect(m_sheets, &CharacterSheetModel::characterSheetHasBeenAdded, this, &IntegrityController::checkIntegrity);
    connect(m_sheets, &CharacterSheetModel::checkIntegrity, this, &IntegrityController::checkAll);
    emit sheetsChanged();
}

void IntegrityController::fieldRemoved(const QString& id)
{
    auto const& list= m_sheets->sheets();
    for(auto sheet : list)
    {
        sheet->removeField(id);
    }
}

void IntegrityController::fieldAdded(CSItem* item)
{
    auto const& list= m_sheets->sheets();
    for(auto sheet : list)
    {
        sheet->insertCharacterItem(item);
    }

    if(item->fieldType() == CSItem::TypeField::TABLE)
    {
        auto table= dynamic_cast<TableFieldController*>(item);
        if(!table)
            return;

        auto updateField= [this]()
        {
            auto table= qobject_cast<TableFieldController*>(sender());

            if(!table)
                return;

            auto id= table->id();

            QJsonObject obj;
            table->saveDataItem(obj);

            auto const& list= m_sheets->sheets();
            for(auto sheet : list)
            {
                auto field= sheet->getFieldFromKey(id);
                auto tableField= dynamic_cast<TableFieldController*>(field);
                if(!tableField)
                    continue;
                tableField->loadDataItem(obj);
            }
        };
        connect(table, &TableFieldController::displayedRowChanged, this, updateField);
        connect(table, &TableFieldController::columnCountChanged, this, updateField);
    }
}

void IntegrityController::checkAll()
{
    auto const& list= m_sheets->sheets();
    for(auto sheet : list)
    {
        checkIntegrity(sheet);
    }
}

void IntegrityController::checkIntegrity(CharacterSheet* sheet)
{
    QSet<QString> validFieldIds;

    auto children= m_field->allChildren();
    for(auto field : children)
    {
        auto path= field->path();
        auto id= field->id();

        validFieldIds << path << id;

        auto sheetField= sheet->getFieldFromKey(id);
        auto sheetFieldPath= sheet->getFieldFromKey(path);

        if(sheetField == sheetFieldPath && sheetFieldPath)
            continue;

        if(sheetField == nullptr && sheetFieldPath)
            sheetFieldPath->setId(id);

        if(sheetFieldPath == nullptr && !sheetField)
            qWarning() << "Item can be found from id but not from path";

        if(sheetField == nullptr && sheetFieldPath == nullptr)
        {
            field->fieldType() == FieldController::TABLE ?
                sheet->insertCharacterItem(copyField<TableFieldController>(field)) :
                sheet->insertCharacterItem(copyField<FieldController>(field));
        }
    }

    for(auto field : children)
    {
        if(field->fieldType() != FieldController::TABLE)
            continue;
        auto id= field->id();
        auto sheetField= sheet->getFieldFromKey(id);
        qDebug() << field->childrenCount() << sheetField->childrenCount();
        if(!sheetField)
        {
            qDebug() << "No valid Table field";
            continue;
        }

        auto sameCount= field->childrenCount() <= sheetField->childrenCount();
        if(sameCount)
            continue;
        auto table= dynamic_cast<TableFieldController*>(sheetField);
        auto origin= dynamic_cast<TableFieldController*>(field);
        if(!table || !origin)
            continue;
        auto originModel= origin->model();
        auto model= table->model();
        QJsonObject obj;
        originModel->save(obj);
        model->load(obj, table);
    }
}

void IntegrityController::checkField() {}
