#include "charactercontroller.h"

#include "charactermodel.h"

CharacterController::CharacterController()
: m_characterModel(new CharacterSheetModel)
{
    m_addCharacter= new QAction(tr("Add character"), this);
    m_deleteCharacter= new QAction(tr("Delete character"), this);
    m_copyCharacter= new QAction(tr("Copy character"), this);
    m_defineAsTabName= new QAction(tr("Character's Name"), this);

    m_applyValueOnAllCharacterLines= new QAction(tr("Apply on all lines"), this);
    m_applyValueOnSelectedCharacterLines= new QAction(tr("Apply on Selection"), this);
    m_applyValueOnAllCharacters= new QAction(tr("Apply on all characters"), this);


    connect(m_addCharacter, &QAction::triggered,
       [&]() { m_undoStack.push(new AddCharacterCommand(m_characterModel)); });
}





void MainWindow::menuRequested(const QPoint& pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);
    QModelIndex index= ui->m_characterView->currentIndex();
    menu.addAction(m_addCharacter);
    // menu.addAction(m_copyCharacter);
    menu.addSeparator();
    menu.addAction(m_applyValueOnAllCharacters);
    menu.addAction(m_applyValueOnSelectedCharacterLines);
    //  menu.addAction(m_applyValueOnAllCharacterLines);
    menu.addAction(m_defineAsTabName);
    menu.addSeparator();
    menu.addAction(m_deleteCharacter);
    QAction* act= menu.exec(QCursor::pos());
    if(act == m_deleteCharacter)
    {
        DeleteCharacterCommand* cmd= new DeleteCharacterCommand(index, m_characterModel);
        m_undoStack.push(cmd);
    }
    else if(act == m_defineAsTabName)
    {
        QString name= index.data().toString();
        if(!name.isEmpty())
        {
                        
CharacterSheet* sheet= m_characterModel->getCharacterSheet(index.column() - 1);
            sheet->setName(name);
        }
    }
    else if(act == m_applyValueOnAllCharacters)
    {
        QString value= index.data().toString();
        QString formula= index.data(Qt::UserRole).toString();
        auto characterItem= m_characterModel->indexToSection(index);
        if((!value.isEmpty()) && (nullptr != characterItem))
        {
            QString key= characterItem->getId();
            SetPropertyOnCharactersCommand* cmd
                = new SetPropertyOnCharactersCommand(key, value, formula, m_characterModel);
            m_undoStack.push(cmd);
        }
    }
    else if(act == m_applyValueOnSelectedCharacterLines)
    {
        applyValueOnCharacterSelection(index, true, false);
    } /// TODO these functions
    else if(act == m_applyValueOnAllCharacterLines)
    {
    }
    else if(act == m_copyCharacter)
    {
    }
}

void CharacterController::setRootSection(Section* section)
{
	m_characterModel->setRootSection(section);
}

void CharacterController::save(QJsonObject obj, bool complete)
{
	m_characterModel->writeModel(obj, complete);
}

void CharacterController::load(QJsonObject obj, bool complete)
{
	m_characterModel->readModeln(obj, complete);
}

void CharacterController::model() const
{
	return m_characterModel.get();
}
