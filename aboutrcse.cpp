#include "aboutrcse.h"
#include "ui_aboutrcse.h"

AboutRcse::AboutRcse(QString version, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutRcse),
    m_version(version)
{
    ui->setupUi(this);


    QString text("Rolisteam Character Sheet Editor v%1<br/><br/>"
                 "This software is dedicated to create character sheet for Rolisteam v%2 and more.<br/>"
                 "Please check the documentation at: wiki.rolisteam.org");
    ui->label->setText(text.arg(m_version).arg(m_version));
}

AboutRcse::~AboutRcse()
{
    delete ui;
}

QString AboutRcse::version() const
{
    return m_version;
}

void AboutRcse::setVersion(const QString &version)
{
    m_version = version;
}
