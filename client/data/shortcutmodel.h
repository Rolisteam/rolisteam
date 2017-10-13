
#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#include <QAbstractItemModel>




class ShortCut
{
public:

private:
	QSequence m_seq;
	QString m_name;
};

class Category
{
public:

private:
 QList<ShortCut> m_shortcuts;
};

class ShortCutModel : public QAbstractItemModel
{
public:
	ShortCutModel();

	
	QVariant data(QModelIndex& index, int role) const;

private:
	QList<Category> m_root;
};

#endif
