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

}

QModelIndex CharacterModel::index(int row, int column, const QModelIndex &parent) const
{

}

QModelIndex CharacterModel::parent(const QModelIndex &child) const
{

}

int CharacterModel::rowCount(const QModelIndex &parent) const
{

}

int CharacterModel::columnCount(const QModelIndex &parent) const
{

}

QVariant CharacterModel::headerData(int section, Qt::Orientation orientation, int role) const
{

}

bool CharacterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

void CharacterModel::appendField(Field *f)
{

}

Qt::ItemFlags CharacterModel::flags(const QModelIndex &index) const
{

}

void CharacterModel::save(QJsonObject &json, bool exp)
{

}

void CharacterModel::load(QJsonObject &json, QGraphicsScene *scene)
{

}

void CharacterModel::generateQML(QTextStream &out, Item::QMLSection sec)
{

}

QString CharacterModel::getValue(const QString &key)
{

}
