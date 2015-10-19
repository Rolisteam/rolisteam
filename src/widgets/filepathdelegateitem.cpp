#include "filepathdelegateitem.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QFileDialog>


#include "preferences/preferencesmanager.h"

ImagePathEditor::ImagePathEditor(QWidget* parent)
	: QWidget(parent)
{
    setUi();
}
ImagePathEditor::~ImagePathEditor()
{

}

void ImagePathEditor::focusInEvent(QFocusEvent * event)
{
    QWidget::focusInEvent(event);
}

void ImagePathEditor::setUi()
{

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->setMargin(0);
    hbox->setSpacing(0);


	setLayout(hbox);

	m_photoBrowser=new QPushButton(QPixmap(),"");

	m_photoLabel=new QLabel();
	m_photoLabel->setScaledContents(true);

	hbox->addWidget(m_photoLabel,1);
	hbox->addWidget(m_photoBrowser);

	connect(m_photoBrowser,SIGNAL(pressed()),this,SLOT(getFileName()));


}


void ImagePathEditor::setPixmap(QPixmap str)
{
	m_pixmap = str;
	m_photoLabel->setPixmap(m_pixmap);
}




QPixmap& ImagePathEditor::getData()
{
	return m_pixmap;
}

void ImagePathEditor::mouseReleaseEvent(QMouseEvent * /* event */)
{
    //emit editingFinished();
}
void ImagePathEditor::getFileName()
{
	/// @todo : 
	PreferencesManager* preferences = PreferencesManager::getInstance();

	QString fileName = QFileDialog::getOpenFileName(this,tr("Get picture for Character State"),
													preferences->value("ImageDirectory",QDir::homePath()).toString(),
													preferences->value("ImageFileFilter","*.jpg *jpeg *.png *.bmp *.svg").toString());

	if (! fileName.isNull() )
    {
		QPixmap pix(fileName);
		if(!pix.isNull())
		{
			m_pixmap = pix;
			m_photoLabel->setPixmap(m_pixmap);
		}
    }
}

//////////////////////////////////////////////
// Method of FilePathDelegateItem
//////////////////////////////////////////////
FilePathDelegateItem::FilePathDelegateItem(QObject *parent):
    QStyledItemDelegate(parent)
{

}

QWidget* FilePathDelegateItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index	) const
{

	ImagePathEditor*	editor = new ImagePathEditor(parent);
   
    return editor;

}
void FilePathDelegateItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.isValid())
    {
		ImagePathEditor* ImgEditor= qobject_cast<ImagePathEditor*>(editor);
		if(NULL!=ImgEditor)
        {
			ImgEditor->setPixmap(index.data(Qt::DisplayRole).value<QPixmap>());
        }
    }

    QStyledItemDelegate::setEditorData(editor, index);

}
void FilePathDelegateItem::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor,option,index);
}
void FilePathDelegateItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    ImagePathEditor* seqEditor= qobject_cast<ImagePathEditor*>(editor) ;
    if(NULL!=seqEditor)
    {
		model->setData(index,m_pix, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }

}
void FilePathDelegateItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    painter->save();

    if(!index.isValid())
        return;

    QStyleOptionViewItemV4 opt = option;
    QStyledItemDelegate::initStyleOption(&opt, index);

    //painter->fillRect(option.rect,option.palette.color(QPalette::Background));


    if(index.isValid())
    {

    }
     painter->restore();
}
QSize FilePathDelegateItem::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize a = QStyledItemDelegate::sizeHint(option,index);
    a.setHeight(30);
    return a;
}

