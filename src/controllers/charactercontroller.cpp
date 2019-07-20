#include "charactercontroller.h"

#include <QAction>
#include <QMenu>
#include <QTreeView>

// undo
#include "undo/addcharactercommand.h"
#include "undo/deletecharactercommand.h"
#include "undo/setpropertyonallcharacters.h"

CharacterController::CharacterController(QUndoStack& undoStack, QTreeView* view, QObject* parent)
    : QObject(parent), m_view(view), m_undoStack(undoStack), m_characterModel(new CharacterSheetModel)
{
    connect(m_view, &QTreeView::customContextMenuRequested, this, &CharacterController::contextMenu);

    m_addCharacter= new QAction(tr("Add character"), this);
    m_deleteCharacter= new QAction(tr("Delete character"), this);
    // m_copyCharacter= new QAction(tr("Copy character"), this);
    m_defineAsTabName= new QAction(tr("Character's Name"), this);

    m_applyValueOnSelectedCharacterLines= new QAction(tr("Apply on Selection"), this);
    m_applyValueOnAllCharacters= new QAction(tr("Apply on all characters"), this);

    connect(m_addCharacter, &QAction::triggered, this, &CharacterController::sendAddCharacterCommand);
    connect(m_deleteCharacter, &QAction::triggered, this, &CharacterController::sendRemoveCharacterCommand);
    connect(m_applyValueOnAllCharacters, &QAction::triggered, this, &CharacterController::applyOnAllCharacter);
    connect(m_applyValueOnSelectedCharacterLines, &QAction::triggered, this, &CharacterController::applyOnSelection);
    connect(m_defineAsTabName, &QAction::triggered, this, [this]() {
        auto index= m_view->currentIndex();
        auto name= index.data().toString();
        if(name.isEmpty())
            return;
        CharacterSheet* sheet= m_characterModel->getCharacterSheet(index.column() - 1);
        sheet->setName(name);
        emit dataChanged();
    });

    connect(m_characterModel.get(), &CharacterSheetModel::characterSheetHasBeenAdded, [this](CharacterSheet* sheet) {
        int col= m_characterModel->columnCount();
        m_view->resizeColumnToContents(col - 2);
        connect(sheet, &CharacterSheet::updateField, m_characterModel.get(), &CharacterSheetModel::fieldHasBeenChanged);
        connect(sheet, &CharacterSheet::addLineToTableField, m_characterModel.get(), &CharacterSheetModel::addSubChild);
        connect(sheet, &CharacterSheet::updateField, this, &CharacterController::dataChanged);
        connect(sheet, &CharacterSheet::addLineToTableField, this, &CharacterController::dataChanged);
        emit dataChanged();
    });
}

void CharacterController::sendAddCharacterCommand()
{
    m_undoStack.push(new AddCharacterCommand(this));
}

void CharacterController::sendRemoveCharacterCommand()
{
    auto idx= m_view->currentIndex();
    if(!idx.isValid())
        return;

    m_undoStack.push(new DeleteCharacterCommand(idx.column() - 1, this));
}

CharacterSheet* CharacterController::characterSheetFromIndex(int index) const
{
    return m_characterModel->getCharacterSheet(index);
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
    sheet->buildDataFromSection(m_characterModel->getRootSection());
    m_characterModel->addCharacterSheet(sheet, pos);
    emit dataChanged();
}

void CharacterController::contextMenu(const QPoint& pos)
{
    Q_UNUSED(pos);
    QMenu menu;
    menu.addAction(m_addCharacter);
    // menu.addAction(m_copyCharacter);
    menu.addSeparator();
    menu.addAction(m_applyValueOnAllCharacters);
    menu.addAction(m_applyValueOnSelectedCharacterLines);
    //  menu.addAction(m_applyValueOnAllCharacterLines);
    menu.addAction(m_defineAsTabName);
    menu.addSeparator();
    menu.addAction(m_deleteCharacter);
    menu.exec(QCursor::pos());
}
void CharacterController::applyOnSelection()
{
    auto index= m_view->currentIndex();
    QVariant var= index.data(Qt::DisplayRole);
    QVariant editvar= index.data(Qt::EditRole);
    if(editvar != var)
    {
        var= editvar;
    }
    int col= index.column();
    QModelIndexList list= m_view->selectionModel()->selectedIndexes();
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
void CharacterController::applyOnAllCharacter()
{
    auto index= m_view->currentIndex();
    QString value= index.data().toString();
    QString formula= index.data(CharacterSheetModel::Formula).toString();
    auto characterItem= m_characterModel->indexToSection(index);
    if((!value.isEmpty()) && (nullptr != characterItem))
    {
        QString key= characterItem->getId();
        SetPropertyOnCharactersCommand* cmd
            = new SetPropertyOnCharactersCommand(key, value, formula, m_characterModel.get());
        m_undoStack.push(cmd);
        emit dataChanged();
    }
}

void CharacterController::setRootSection(Section* section)
{
    m_characterModel->setRootSection(section);
}

void CharacterController::save(QJsonObject& obj, bool complete)
{
    m_characterModel->writeModel(obj, complete);
}

void CharacterController::load(const QJsonObject& obj, bool complete)
{
    m_characterModel->readModel(obj, complete);
}
void CharacterController::clear()
{
    m_characterModel->clearModel();
}

QAbstractItemModel* CharacterController::model() const
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
