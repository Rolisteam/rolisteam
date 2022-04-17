#ifndef FILEPATHDELEGATEITEM_H
#define FILEPATHDELEGATEITEM_H

#include "rwidgets_global.h"
#include <QLabel>
#include <QPushButton>
#include <QStyledItemDelegate>
namespace rwidgets
{
/**
 * @brief The FilePathDelegateItem class displays path for image in the treeview
 */
class RWIDGET_EXPORT ImagePathDelegateItem : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ImagePathDelegateItem(const QString& root, QObject* parent= nullptr);

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    QPixmap m_pix;
    QString m_root;
};
} // namespace rwidgets
#endif // FILEPATHDELEGATEITEM_H
