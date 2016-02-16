#ifndef CHARACTERMODEL_H
#define CHARACTERMODEL_H

#include "fieldmodel.h"
#include <QObject>
#include <QAbstractItemModel>
class Character
{

};

/**
 * @brief The FieldModel class
 */
class CharacterModel : public QAbstractItemModel
{
    //Q_OBJECT
public:
    explicit CharacterModel(QObject *parent = 0);
    virtual ~CharacterModel();

    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void appendField(Field* f);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void save(QJsonObject& json,bool exp=false);
    void load(QJsonObject& json,QGraphicsScene* scene);

    void generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec);

    Q_INVOKABLE QString getValue(const QString& key);



private:
    Section* m_rootSection;
    QList<Character*> m_characterList ;

};


#endif // CHARACTERMODEL_H
