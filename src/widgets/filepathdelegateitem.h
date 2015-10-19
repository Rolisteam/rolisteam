#ifndef FILEPATHDELEGATEITEM_H
#define FILEPATHDELEGATEITEM_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QPushButton>


class imagePathEditor : public QWidget
{
    Q_OBJECT

public:
    imagePathEditor(bool isStereo        ,QWidget* parent = 0);
    ~imagePathEditor();

    void setLeftPath(QString str);
    void setRightPath(QString str);
	void	setRawFormat(bool);
	QVariant getData();
	void	setMedia(Media*);




signals:
     void editingFinished();

public slots:
    void reversePath();
    void browseLeftPath();
    void browseRightPath();
    void setStereoVisible(bool v);
	void slotShowRawDialog();

protected:
    void setUi();
    void getFileName(QString title,QLineEdit* line);
    void mouseReleaseEvent(QMouseEvent * /* event */);
    void focusInEvent(QFocusEvent * event);

signals:
	void showRawDialog(Media*);


private:
    QLineEdit* m_rightFilePath;
    QPushButton* m_leftFilePathBrowser;
    QLineEdit* m_leftFilePath;
    QPushButton* m_rightFilePathBrowser;
    bool m_isStereo;
    QPushButton* m_reversePush;
	QPushButton*	_rawButton;
};

/**
 * @brief The FilePathDelegateItem class - displays path for image in the treeview
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

public slots:

private slots:
	void commitAndCloseEditor();

signals:
	void showRawDialog(Media*);

private:
    QStringList shortDataToSize(QStringList,int size,const QFont& font) const;
	imagePathEditor* m_editor;


};


#endif // FILEPATHDELEGATEITEM_H
