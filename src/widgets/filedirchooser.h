#ifndef DIRCHOOSER_H
#define DIRCHOOSER_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

/**
 * @brief A LineEdit with a button to choose a file or directory. It displays a QLineEdit and a QPushButton allowing to show the
 * current selection and to browse to select something else.
 *
 */
class FileDirChooser : public QWidget
{
    Q_OBJECT


public:
    /**
     * @brief FileDirChooser
     * @param isDirectory
     * @param parent
     */
    FileDirChooser(bool isDirectory = true,QWidget * parent = NULL);
    ~FileDirChooser();
    /**
     * @brief setPath define the value of the widget. Current selection.
     * @param dirname
     */
    void setPath(const QString & dirname);
    /**
     * @brief path
     * @return the path selected by the user.
     */
    QString path() const;
    /**
     * @brief setFilter defines filter to select the file.
     * @param filter
     */
    void setFilter(const QString& filter);
    /**
     * @brief getFilter
     * @return
     */
    QString getFilter();
    /**
     * @brief setMode
     * @param isDirectory
     */
    void setMode(bool isDirectory);


private slots:
    /**
     * @brief browse
     */
    void browse();

private:
    QLineEdit* m_lineEdit;
    QString m_filter;
    bool m_directory;


};

#endif // DIRCHOOSER_H
