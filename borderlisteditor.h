#ifndef BORDERLISTEDITOR_H
#define BORDERLISTEDITOR_H

#include <QStyledItemDelegate>


#include "field.h"

class BorderListEditor : public QStyledItemDelegate
{
    Q_OBJECT



public:
    BorderListEditor(QObject *widget = 0);
    QString displayText(const QVariant & value, const QLocale & locale) const;


    QWidget* createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    void setEditorData(QWidget* editor,const QModelIndex& index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;

private:
    QMap<Field::BorderLine,QString> m_map;

};

#endif // BORDERLISTEDITOR_H
