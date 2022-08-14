#include "charactercontroller.h"

#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QTreeView>

// undo
#include "charactersheet/controllers/section.h"
#include "charactersheet/csitem.h"
#include "undo/addcharactercommand.h"
#include "undo/deletecharactercommand.h"
#include "undo/setpropertyonallcharacters.h"

CharacterController::CharacterController(QObject* parent)
    : QObject(parent)
    , m_characterModel(new CharacterSheetModel)
    , m_characters(new CharacterList(m_characterModel.get()))
{

    // m_characters->setSourceModel(m_characterModel.get());

    connect(m_characterModel.get(), &CharacterSheetModel::characterSheetHasBeenAdded, this,
            [this](CharacterSheet* sheet) {
                connect(sheet, &CharacterSheet::updateField, m_characterModel.get(),
                        &CharacterSheetModel::fieldHasBeenChanged);
                connect(sheet, &CharacterSheet::addLineToTableField, m_characterModel.get(),
                        &CharacterSheetModel::addSubChild);
                connect(sheet, &CharacterSheet::updateField, this, &CharacterController::dataChanged);
                connect(sheet, &CharacterSheet::addLineToTableField, this, &CharacterController::dataChanged);
                emit dataChanged();
            });
}

CharacterController::~CharacterController()= default;

void CharacterController::sendAddCharacterCommand()
{
    emit performCommand(new AddCharacterCommand(this));
}

CharacterList* CharacterController::characters() const
{
    return m_characters.get();
}

void CharacterController::sendRemoveCharacterCommand(const QModelIndex& index)
{
    if(index.column() > 0)
        emit performCommand(new DeleteCharacterCommand(index.column() - 1, this));
}

CharacterSheet* CharacterController::characterSheetFromIndex(int index) const
{
    return m_characterModel->getCharacterSheet(index);
}

CharacterSheet* CharacterController::characterSheetFromUuid(const QString& uuid) const
{
    return m_characterModel->getCharacterSheetById(uuid);
}

void CharacterController::addCharacter()
{
    insertCharacter(characterCount(), new CharacterSheet);
    emit dataChanged();
}
void CharacterController::removeCharacter(int index)
{
    m_characterModel->removeCharacterSheet(index);
    emit dataChanged();
}

void CharacterController::insertCharacter(int pos, CharacterSheet* sheet)
{
    auto root= m_characterModel->getRootSection();
    if(!root || !sheet)
        return;
    root->buildDataInto(sheet);
    m_characterModel->addCharacterSheet(sheet, pos);
    emit dataChanged();
}

void CharacterController::applyOnSelection(const QModelIndex& index, const QModelIndexList& list)
{
    QVariant var= index.data(Qt::DisplayRole);
    QVariant editvar= index.data(Qt::EditRole);
    if(editvar != var)
    {
        var= editvar;
    }
    int col= index.column();
    for(QModelIndex modelIndex : list)
    {
        if(modelIndex.column() == col)
        {
            m_characterModel->setData(modelIndex, var, Qt::EditRole);
        }
    }
    if(!list.isEmpty())
        emit dataChanged();
}
void CharacterController::applyOnAllCharacter(const QModelIndex& index)
{

    QString value= index.data().toString();
    QString formula= index.data(CharacterSheetModel::FormulaRole).toString();
    auto characterItem= m_characterModel->indexToSection(index);
    if((!value.isEmpty()) && (nullptr != characterItem))
    {
        QString key= characterItem->id();
        emit performCommand(new SetPropertyOnCharactersCommand(key, value, formula, m_characterModel.get()));
        emit dataChanged();
    }
}

void CharacterController::setRootSection(Section* section)
{
    m_characterModel->setRootSection(section);
}

void CharacterController::save(QJsonObject& obj)
{
    m_characterModel->writeModel(obj);
}

void CharacterController::load(const QJsonObject& obj, bool complete)
{
    m_characterModel->readModel(obj, complete);
}
void CharacterController::clear()
{
    m_characterModel->clearModel();
}

CharacterSheetModel* CharacterController::model() const
{
    return m_characterModel.get();
}
int CharacterController::characterCount() const
{
    return m_characterModel->getCharacterSheetCount();
}
void CharacterController::checkCharacter(Section* sec)
{
    m_characterModel->checkCharacter(sec);
}
