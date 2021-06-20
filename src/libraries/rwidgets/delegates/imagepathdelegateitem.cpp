#include "imagepathdelegateitem.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>

#include "customs/imagepatheditor.h"
#include "model/characterstatemodel.h"

namespace rwidgets
{

ImagePathDelegateItem::ImagePathDelegateItem(const QString& root, QObject* parent)
    : QStyledItemDelegate(parent), m_root(root)
{
}

QWidget* ImagePathDelegateItem::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    ImagePathEditor* editor= new ImagePathEditor(m_root, parent);

    return editor;
}

void ImagePathDelegateItem::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(index.isValid())
    {
        ImagePathEditor* ImgEditor= qobject_cast<ImagePathEditor*>(editor);
        if(nullptr != ImgEditor)
        {
            ImgEditor->setPixmap(index.data(CharacterStateModel::PICTURE).value<QPixmap>());
        }
    }
    QStyledItemDelegate::setEditorData(editor, index);
}

void ImagePathDelegateItem::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                                                 const QModelIndex& index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void ImagePathDelegateItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    ImagePathEditor* imgEditor= qobject_cast<ImagePathEditor*>(editor);
    if(nullptr != imgEditor)
    {
        QVariant var;
        var.setValue(imgEditor->filename());
        model->setData(index, var, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ImagePathDelegateItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // paint selection
    QStyledItemDelegate::paint(painter, option, index);

    QPixmap pix= index.data().value<QPixmap>();

    if(pix.isNull())
        return;

    QRect rectImg= pix.rect();
    qreal ratioImg= (qreal)rectImg.width() / (qreal)rectImg.height();

    QRect target2= option.rect;
    qreal ratioZone= (qreal)target2.width() / (qreal)target2.height();

    QRect target(target2);

    if(ratioZone >= 1)
    {
        target.setHeight(target2.height());
        target.setWidth(target2.height() * ratioImg);
    }
    else
    {
        target.setWidth(target2.width());
        target.setHeight(target2.width() / ratioImg);
    }

    painter->drawImage(target, pix.toImage());
}

QSize ImagePathDelegateItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize a= QStyledItemDelegate::sizeHint(option, index);
    a.setHeight(30);
    return a;
}
} // namespace rwidgets
