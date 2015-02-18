#ifndef SESSIONITEMMODEL_H
#define SESSIONITEMMODEL_H

#include <QAbstractItemModel>
#include "cleveruri.h"
class RessourcesNode;
class ResourcesItem
{
public:
    /**
      * @brief constructor with parameter
      * @param Section pointer
      * @param isLeaf either the item is a leaf or not.
      */
    ResourcesItem(RessourcesNode* p,bool isLeaf);

    /**
      * @brief defines the stored section
      */
    void setData(RessourcesNode* p);
    /**
      * @brief accessor to the stored section
      */
    RessourcesNode* getData();



    /**
      * @brief return either the item is a leaf or note.
      */
    bool isLeaf();
    /**
      * @brief defines if the item is a leaf or not
      */
    void setLeaf(bool leaf);
    /**
      * @brief get the item's parent
      * @return pointer to its parent
      */
    ResourcesItem* getParent();
    /**
      * @brief defines the item's parent
      * @param new parent
      */
    void setParent(ResourcesItem* p);
    /**
      * @brief adds child in this item.
      * @param new child
      */
    void addChild(ResourcesItem* child);
    /**
      * @brief gives the child at the given position
      * @param the position (row) of the wanted item.
      * @return TreeItem's pointer or NULL if row is out of range.
      */
    ResourcesItem* child(int row);
    /**
      * @brief gives the position of the current item into is father.
      */
    int row();
    /**
      * @brief gives the position of the given child in the children list.
      * @param the child
      * @return position of child, -1 if not matched.
      */
    int indexOfChild(ResourcesItem* itm);
    /**
      * @brief accessor to the children count.
      */
    int childrenCount();

    void clean();
private:
    RessourcesNode* m_data;
    bool m_isLeaf; /// @brief true when no child and has valid parent
    bool m_directory; // organisation helper.
    ResourcesItem* m_parent;
    QList<ResourcesItem*>* m_children;

};

class Session;
class Chapter;
class SessionItemModel : public QAbstractItemModel
{
public:
    SessionItemModel();

   virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   virtual QModelIndex parent( const QModelIndex & index ) const;
   virtual int rowCount(const QModelIndex&) const;
   virtual int columnCount( const QModelIndex&) const;
   virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
   QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    Chapter* addChapter(QString& name);
    void setSession(Session* s);
    void addChapter(Chapter* c );
    CleverURI* addRessources(QString& urifile, CleverURI::ContentType& type,QModelIndex& index);

    void populateChapter(Chapter& t,ResourcesItem* parentIte);
private:
    //Section* m_rootSection;

    ResourcesItem* m_rootItem;
    Session* m_session;
};

#endif // SESSIONITEMMODEL_H
