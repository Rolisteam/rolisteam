/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include "persondialog.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

PersonDialog::PersonDialog(QWidget * parent)
    : QDialog(parent)
{
    setUI();
}

QString PersonDialog::getName() const
{
    return m_name_w->text();
}

QColor PersonDialog::getColor() const
{
    return m_color_w->color();
}

int PersonDialog::edit(QString title, QString name, QColor color)
{
    setWindowTitle(title);
    m_name_w->setText(name);
    m_color_w->setColor(color);
    return exec();
}


void PersonDialog::setUI()
{
    m_name_w = new QLineEdit(this);
    m_color_w = new ColorButton(QColor("white"), this);

    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("&Nom : "), m_name_w);
    formLayout->addRow(tr("&Couleur : "), m_color_w);

    QPushButton* okButton = new QPushButton(tr("OK"));
    okButton->setDefault(true);

    QPushButton* cancelButton = new QPushButton(tr("&Cancel"));
    cancelButton->setAutoDefault(false);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);
    buttonBox->addButton(okButton,QDialogButtonBox::AcceptRole);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}
