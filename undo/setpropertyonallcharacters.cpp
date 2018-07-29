#include "setpropertyonallcharacters.h"

SetPropertyOnCharactersCommand::SetPropertyOnCharactersCommand(QString idProperty, QString value, QString formula, CharacterSheetModel* model,QUndoCommand *parent)
    : QUndoCommand(parent),m_model(model), m_value(value),m_formula(formula),m_propertyId(idProperty)
{
    for(int i = 0 ; i < m_model->getCharacterSheetCount();++i)
    {
      auto charact = m_model->getCharacterSheet(i);
      if(nullptr != charact)
      {
        m_oldValues << charact->getValue(m_propertyId,Qt::DisplayRole).toString();
        m_oldFormula << charact->getValue(m_propertyId,Qt::UserRole).toString();
      }
    }
    setText(QObject::tr("Set property on all characters: %1").arg(idProperty));
}

void SetPropertyOnCharactersCommand::undo()
{
    for(int i = 0 ; i < m_model->getCharacterSheetCount();++i)
    {
      auto charact = m_model->getCharacterSheet(i);
      if(nullptr != charact)
      {
        charact->setValue(m_propertyId,m_oldValues.at(i),m_oldFormula.at(i));
      }
    }
}

void SetPropertyOnCharactersCommand::redo()
{
    for(int i = 0 ; i < m_model->getCharacterSheetCount();++i)
    {
      auto charact = m_model->getCharacterSheet(i);
      if(nullptr != charact)
      {
        charact->setValue(m_propertyId,m_value,m_formula);
      }
    }
}
