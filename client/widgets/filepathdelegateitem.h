#ifndef FILEPATHDELEGATEITEM_H
#define FILEPATHDELEGATEITEM_H

#include <QStyledItemDelegate>
#include <QPushButton>
#include <QLabel>

/**
 * @brief The ImagePathEditor class get the path to image.
 */
class ImagePathEditor : public QWidget
{
    Q_OBJECT

public:
    ImagePathEditor(QWidget* parent = 0);
    ~ImagePathEditor();

    void setPixmap(QPixmap );

    QPixmap getData();


signals:
    void editingFinished();

public slots:
    void getFileName();
    void readPixmap(QString str);
    void clearPixmap();

protected:
    void setUi();

    void mouseReleaseEvent(QMouseEvent * /* event */);
    void focusInEvent(QFocusEvent * event);

private:
    QLineEdit* m_photoEdit;
    QPushButton* m_photoBrowser;
    QPushButton* m_cleanButton;
    QPixmap m_pixmap;
};

/**
 * @brief The FilePathDelegateItem class displays path for image in the treeview
 */
class FilePathDelegateItem : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FilePathDelegateItem(QObject *parent = 0);

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index	) const;
    void setEditorData(QWidget *editor	, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

private:
    QPixmap m_pix;


};


#endif // FILEPATHDELEGATEITEM_H
