#include "filedirchooser.h"

#include <QDir>
#include <QHBoxLayout>
#include <QFileDialog>

/**************
 * FileDirChooser *
 **************/

FileDirChooser::FileDirChooser(bool isDirectory ,QWidget * parent)
 : QWidget(parent),m_directory(isDirectory)
{


    // Childrens
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setText(QDir::homePath());
    QPushButton * button = new QPushButton(tr("..."), this);

    // Layout
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(m_lineEdit, 1);
    layout->addWidget(button, 0);
    setLayout(layout);

    // Connections
    connect(button, SIGNAL(clicked()), this, SLOT(browse()));

    // Misc
    button->setMaximumWidth(28);
    setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);
}

FileDirChooser::~FileDirChooser()
{
    // QObject should do it right for us already.
}
void FileDirChooser::setMode(bool isDirectory)
{
    m_directory=isDirectory;
}

void FileDirChooser::setPath(const QString & path)
{
    m_lineEdit->setText(path);
}

QString FileDirChooser::path() const
{
    return m_lineEdit->text();
}
void FileDirChooser::setFilter(const QString& filter)
{
    m_filter=filter;
}

QString FileDirChooser::getFilter()
{
    return m_filter;
}

void FileDirChooser::browse()
{
     QString result;
    if(m_directory)
    {
    result = QFileDialog::getExistingDirectory(this,
            tr("Select directory"),
            m_lineEdit->text(),
            QFileDialog::ShowDirsOnly  | QFileDialog::DontResolveSymlinks);
    }
    else
    {
        result = QFileDialog::getOpenFileName(this,
                tr("Open File"),
                m_lineEdit->text(),
                m_filter );
    }
    if (!result.isEmpty())
    {
        m_lineEdit->setText(result);
    }
}
