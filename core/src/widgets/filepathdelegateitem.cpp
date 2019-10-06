#include "filepathdelegateitem.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>

#include "preferences/preferencesmanager.h"

ImagePathEditor::ImagePathEditor(QWidget* parent) : QWidget(parent)
{
    setUi();
}
ImagePathEditor::~ImagePathEditor() {}

void ImagePathEditor::focusInEvent(QFocusEvent* event)
{
    QWidget::focusInEvent(event);
}

void ImagePathEditor::setUi()
{
    QHBoxLayout* hbox= new QHBoxLayout();
    hbox->setMargin(0);
    hbox->setSpacing(0);

    setLayout(hbox);

    m_photoBrowser= new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), "");

    m_photoEdit= new QLineEdit();

    m_cleanButton= new QPushButton(QIcon(":/resources/icons/delete.png"), "");

    hbox->addWidget(m_photoEdit, 1);
    hbox->addWidget(m_photoBrowser);
    hbox->addWidget(m_cleanButton);

    connect(m_photoBrowser, SIGNAL(pressed()), this, SLOT(getFileName()));
    connect(m_photoEdit, SIGNAL(textEdited(QString)), this, SLOT(readPixmap(QString)));
    connect(m_cleanButton, SIGNAL(pressed()), this, SLOT(clearPixmap()));
}

void ImagePathEditor::setPixmap(QPixmap str)
{
    m_pixmap= str;
    // m_photoLabel->setPixmap(m_pixmap);
}

QPixmap ImagePathEditor::getData()
{
    return m_pixmap;
}

void ImagePathEditor::mouseReleaseEvent(QMouseEvent* /* event */)
{
    // emit editingFinished();
}
void ImagePathEditor::clearPixmap()
{
    m_pixmap= QPixmap();
}

void ImagePathEditor::getFileName()
{
    PreferencesManager* preferences= PreferencesManager::getInstance();

    QString fileName= QFileDialog::getOpenFileName(this, tr("Get picture for Character State"),
        preferences->value("StateImageDirectory", QDir::homePath()).toString(),
        preferences->value("ImageFileFilter", "*.jpg *jpeg *.png *.bmp *.svg").toString());
    if(!fileName.isEmpty())
    {
        readPixmap(fileName);

        QFileInfo info(fileName);
        preferences->registerValue("StateImageDirectory", info.absolutePath());
    }
}
void ImagePathEditor::readPixmap(QString str)
{
    if(!str.isEmpty())
    {
        QPixmap pix(str);
        if(!pix.isNull())
        {
            m_pixmap= pix;
            m_photoEdit->setText(str);
        }
    }
}

//////////////////////////////////////////////
// Method of FilePathDelegateItem
//////////////////////////////////////////////
FilePathDelegateItem::FilePathDelegateItem(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget* FilePathDelegateItem::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    ImagePathEditor* editor= new ImagePathEditor(parent);

    return editor;
}
void FilePathDelegateItem::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(index.isValid())
    {
        ImagePathEditor* ImgEditor= qobject_cast<ImagePathEditor*>(editor);
        if(nullptr != ImgEditor)
        {
            ImgEditor->setPixmap(index.data(Qt::DisplayRole).value<QPixmap>());
        }
    }
    QStyledItemDelegate::setEditorData(editor, index);
}
void FilePathDelegateItem::updateEditorGeometry(
    QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}
void FilePathDelegateItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    ImagePathEditor* imgEditor= qobject_cast<ImagePathEditor*>(editor);
    if(nullptr != imgEditor)
    {
        QVariant var;
        var.setValue(imgEditor->getData());
        model->setData(index, var, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
void FilePathDelegateItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
QSize FilePathDelegateItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize a= QStyledItemDelegate::sizeHint(option, index);
    a.setHeight(30);
    return a;
}
