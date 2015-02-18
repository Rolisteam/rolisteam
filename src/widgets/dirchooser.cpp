#include "dirchooser.h"

#include <QDir>
#include <QHBoxLayout>
#include <QFileDialog>

/**************
 * DirChooser *
 **************/

DirChooser::DirChooser(QWidget * parent)
 : QWidget(parent)
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
    connect(button, SIGNAL(pressed()), this, SLOT(browse()));

    // Misc
    button->setMaximumWidth(28);
    setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);
}

DirChooser::~DirChooser()
{
    // QObject should do it right for us already.
}

void DirChooser::setDirName(const QString & dirName)
{
    m_lineEdit->setText(dirName);
}

QString DirChooser::dirName() const
{
    return m_lineEdit->text();
}

void DirChooser::browse()
{
    QString result = QFileDialog::getExistingDirectory(this,
            tr("Select directory"),
            m_lineEdit->text(),
            QFileDialog::ShowDirsOnly );
    if (!result.isEmpty())
    {
        m_lineEdit->setText(result);
    }
}
