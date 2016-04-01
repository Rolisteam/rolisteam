#include "charactermodel.h"

CharacterModel::CharacterModel(QObject *parent)
    : QAbstractItemModel(parent)
{

}

CharacterModel::~CharacterModel()
{

}



QVariant CharacterModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QModelIndex CharacterModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}

QModelIndex CharacterModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int CharacterModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int CharacterModel::columnCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant CharacterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool CharacterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

void CharacterModel::appendField(Field *f)
{

}

Qt::ItemFlags CharacterModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled;
}

void CharacterModel::save(QJsonObject &json, bool exp)
{

}

void CharacterModel::load(QJsonObject &json, QGraphicsScene *scene)
{

}

void CharacterModel::generateQML(QTextStream &out, CharacterSheetItem::QMLSection sec)
{

}

QString CharacterModel::getValue(const QString &key)
{
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
