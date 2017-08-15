#include "charactermodel.h"

CharacterModel::CharacterModel(QObject *parent)
    : QAbstractItemModel(parent)
{

}

CharacterModel::~CharacterModel()
{

}



QVariant CharacterModel::data(const QModelIndex &, int ) const
{
    return QVariant();
}

QModelIndex CharacterModel::index(int , int , const QModelIndex &) const
{
    return QModelIndex();
}

QModelIndex CharacterModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int CharacterModel::rowCount(const QModelIndex &) const
{
    return 0;
}

int CharacterModel::columnCount(const QModelIndex &) const
{
    return 0;
}

QVariant CharacterModel::headerData(int , Qt::Orientation , int ) const
{
    return QVariant();
}

bool CharacterModel::setData(const QModelIndex &, const QVariant &, int )
{
    return false;
}

void CharacterModel::appendField(Field *f)
{
  Q_UNUSED(f);
}

Qt::ItemFlags CharacterModel::flags(const QModelIndex &index) const
{
      Q_UNUSED(index);
    return Qt::ItemIsEnabled;
}

void CharacterModel::save(QJsonObject &json, bool exp)
{
  Q_UNUSED(json);
      Q_UNUSED(exp);
}

void CharacterModel::load(QJsonObject &json, QGraphicsScene *scene)
{
  Q_UNUSED(json);
      Q_UNUSED(scene);
}

void CharacterModel::generateQML(QTextStream &out, CharacterSheetItem::QMLSection sec)
{
  Q_UNUSED(out);
      Q_UNUSED(sec);
}

QString CharacterModel::getValue(const QString &key)
{
      Q_UNUSED(key);
    return QString();
}

void CharacterModel::setRootSection(Section *rootSection)
{
    m_rootSection = rootSection;
}

void CharacterModel::addCharacter()
{
    beginInsertColumns(QModelIndex(),1+m_characterList.size(),1+m_characterList.size());
    m_characterList.append(new CharacterSheet());
    endInsertColumns();
}
