#include "borderlisteditor.h"

#include <QListWidget>

BorderListEditor::BorderListEditor(QObject *widget)
    : QStyledItemDelegate(widget)
{
    m_map.insert(Field::UP,tr("Up"));
    m_map.insert(Field::LEFT,tr("Left"));
    m_map.insert(Field::DOWN,tr("Down"));
    m_map.insert(Field::RIGHT,tr("Right"));
}

QString BorderListEditor::displayText(const QVariant &value, const QLocale &locale) const
{
    int border = value.toInt();

    QString result;
    if(border==15)
    {
        result=tr("All");
    }
    else if(border==16)
    {
        result=tr("None");
    }
    else
    {
        QStringList list;
        foreach (Field::BorderLine line, m_map.keys())
        {
            if(border & line)
            {
                list << m_map[line];
            }
        }
        result = list.join(',');
    }
    return result;
}

QWidget *BorderListEditor::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QListWidget* cb = new QListWidget(parent);
    cb->setSelectionMode(QAbstractItemView::ExtendedSelection);
    cb->setMinimumHeight(80);
    return cb;
}

void BorderListEditor::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QListWidget* list = qobject_cast<QListWidget*>(editor);
    int i = index.data().toInt();

    foreach (Field::BorderLine line, m_map.keys())
    {
        QListWidgetItem* item = new QListWidgetItem(list);
        item->setData(Qt::UserRole,(int)line);
        item->setText(m_map[line]);
        item->setSelected(i & line);
    }

}

void BorderListEditor::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QListWidget* list = qobject_cast<QListWidget*>(editor);
    int value = 0;
    QList<QListWidgetItem*> selection = list->selectedItems();
    if(selection.isEmpty())
    {
        value = 16;
    }
    else if(selection.size() == m_map.size())
    {
        value = 15;
    }
    else
    {
        foreach(QListWidgetItem* item,selection)
        {
            value += item->data(Qt::UserRole).toInt();
        }

    }
    model->setData(index,value);
    QStyledItemDelegate::setEditorData(editor, index);
}
