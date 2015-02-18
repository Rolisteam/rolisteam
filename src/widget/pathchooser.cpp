#include "pathchooser.h"
#include <QFileDialog>

PathChooser::PathChooser(QWidget *parent) :
    QWidget(parent)
{
    m_button = new QPushButton(tr("Browse"));
    connect(m_button,SIGNAL(pressed()),this,SLOT(onButton()));
    m_line= new QLineEdit();
    connect(m_line,SIGNAL(textChanged(QString)),this,SLOT(editPath()));

    setupUi();
}
//QString& title,QString& filter,
void PathChooser::setTitle(QString& t)
{
    m_title=t;
}

void PathChooser::setFilters(QString& t)
{
    m_filters=t;
}

void PathChooser::setPath(QString m)
{
    m_path = m;
    m_line->setText(m_path);
}
void PathChooser::setupUi()
{
    m_layout = new QHBoxLayout;
    m_layout->setMargin(0);
    m_layout->addWidget(m_line);
    m_layout->addWidget(m_button);

    setLayout(m_layout);

}
void PathChooser::editPath()
{
    m_path =m_line->text();

    emit pathChanged();
}

void PathChooser::onButton()
{
    if(m_isDirectory)
        m_path = QFileDialog::getExistingDirectory(this, m_title, m_path);
    else
        m_path = QFileDialog::getOpenFileName(this, m_title,m_path,m_filters);

    m_line->setText(m_path);
    emit pathChanged();
}

QString PathChooser::getPath()
{
    return m_path;
}
