#include "FilePathDelegateItem.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QApplication>
#include <QtGui/QPainter>
#include <QtCore/QDebug>


ImagePathEditor::ImagePathEditor(bool isStereo,QWidget* parent)
	: QWidget(parent),m_isStereo(isStereo),
	  _media(0x0)
{
    setUi();
    setMouseTracking(true);
}
ImagePathEditor::~ImagePathEditor()
{

}

void ImagePathEditor::focusInEvent(QFocusEvent * event)
{
    QWidget::focusInEvent(event);
    m_leftFilePath->setCursorPosition(m_leftFilePath->text().size());
    m_leftFilePath->deselect();
}

void ImagePathEditor::setStereoVisible(bool visible)
{
    if(NULL!=m_rightFilePath)
    {
        m_rightFilePath->setVisible(visible);
    }
    if(NULL!=m_rightFilePathBrowser)
    {
        m_rightFilePathBrowser->setVisible(visible);
    }
    if(NULL!=m_reversePush)
    {
        m_reversePush->setVisible(visible);
    }
}

void ImagePathEditor::slotShowRawDialog()
{
	emit showRawDialog(_media);
}

void ImagePathEditor::setUi()
{

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->setMargin(0);
    hbox->setSpacing(0);


	setLayout(hbox);

    m_leftFilePathBrowser=new QPushButton(QPixmap( ( const char** ) FileOpen_xpm ),"");
            //QPushButton(style->standardIcon(QStyle::SP_DialogOpenButton),"");
    m_leftFilePath=new QLineEdit();
    connect(m_leftFilePath,SIGNAL(editingFinished()),this,SIGNAL(editingFinished()));

    hbox->addWidget(m_leftFilePathBrowser);
    hbox->addWidget(m_leftFilePath);

    connect(m_leftFilePathBrowser,SIGNAL(pressed()),this,SLOT(browseLeftPath()));

	_rawButton = new QPushButton(this);
	_rawButton->setFixedSize( QSize( 28,26 ) );
	_rawButton->setIconSize( QSize( 24,24 ) );
	_rawButton->setIcon(QIcon(QPixmap(Raw_xpm)));
	_rawButton->setToolTip("RAW format options");
	hbox->addWidget(_rawButton);
	connect(_rawButton,SIGNAL(pressed()),this,SLOT(slotShowRawDialog()));
}
void ImagePathEditor::reversePath()
{
    QString str = m_rightFilePath->text();
    m_rightFilePath->setText(m_leftFilePath->text());
    m_leftFilePath->setText(str);

	emit editingFinished();
}

void ImagePathEditor::setLeftPath(QString str)
{
    m_leftFilePath->setText(str);

}

void ImagePathEditor::setRightPath(QString str)
{
    m_rightFilePath->setText(str);
}


QVariant ImagePathEditor::getData()
{
    if(m_isStereo)
    {
        QStringList list;
        list << m_leftFilePath->text();
        list << m_rightFilePath->text();
        return list;
    }
    else
    {
        return m_leftFilePath->text();
    }

}

void ImagePathEditor::setMedia(Media * aMedia)
{
	_media = aMedia;
}

void ImagePathEditor::browseLeftPath()
{
    getFileName(m_leftFilePath->text(),m_leftFilePath);
}

void ImagePathEditor::browseRightPath()
{
    getFileName(m_rightFilePath->text(),m_rightFilePath);
}
void ImagePathEditor::mouseReleaseEvent(QMouseEvent * /* event */)
{
    //emit editingFinished();
}
void ImagePathEditor::getFileName(QString title,QLineEdit* line)
{
	/// @todo : 

    if ( m_fileDialog->exec() == QDialog::Accepted )
    {
        QString selectedFile(m_fileDialog->selectedFiles().at(0).name().string().c_str());
        line->setText(selectedFile);
        emit editingFinished();
    }
}

//////////////////////////////////////////////
// Method of FilePathDelegateItem
//////////////////////////////////////////////
FilePathDelegateItem::FilePathDelegateItem(QObject *parent):
    QStyledItemDelegate(parent)
{
	connect( m_editor, SIGNAL(showRawDialog(Media*)), this, SIGNAL(showRawDialog(Media*)));
}

QWidget* FilePathDelegateItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index	) const
{

    ImagePathEditor*	editor = new ImagePathEditor(true);
   


    return editor;

}
void FilePathDelegateItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.isValid())
    {
        ImagePathEditor* seqEditor= qobject_cast<ImagePathEditor*>(editor);
        if(NULL!=seqEditor)
        {
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
		model->setData(index,seqEditor->getData(), Qt::EditRole);
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
QStringList FilePathDelegateItem::shortDataToSize(QStringList list, int size,const QFont& font) const
{

}
void FilePathDelegateItem::commitAndCloseEditor()
{
}
}
