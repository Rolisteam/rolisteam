/***************************************************************************
    *   Copyright (C) 2016 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                                      *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    *   This program is distributed in the hope that it will be useful,       *
    *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
    *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
    *   GNU General Public License for more details.                          *
    *                                                                         *
    *   You should have received a copy of the GNU General Public License     *
    *   along with this program; if not, write to the                         *
    *   Free Software Foundation, Inc.,                                       *
    *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
    ***************************************************************************/
#include "persondialog.h"
#include "ui_persondialog.h"


#include <QFileDialog>


PersonDialog::PersonDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PersonDialog)
{
    ui->setupUi(this);

    connect(ui->m_selectCharaterAvatar,SIGNAL(clicked()),this,SLOT(openImage()));
}

PersonDialog::~PersonDialog()
{
    delete ui;
}
QString PersonDialog::getName() const
{
    return ui->m_characterName->text();
}

QColor PersonDialog::getColor() const
{
    return ui->m_characterColor->color();
}
QString  PersonDialog::getAvatarUri() const
{
    return m_avatar;
}
void PersonDialog::openImage()
{
    m_avatar = QFileDialog::getOpenFileName(this,tr("Load Avatar"));

    if(!m_avatar.isEmpty())
    {
        ui->m_selectCharaterAvatar->setIcon(QIcon(QPixmap::fromImage(QImage(m_avatar))));
    }
}
int  PersonDialog::edit(QString title, QString name, QColor color)
{

 setWindowTitle(title);
 ui->m_characterName->setText(name);
 ui->m_characterColor->setColor(color);


 return exec();
}
void PersonDialog::setVisible(bool visible)
{
    ui->m_characterName->selectAll();
    QDialog::setVisible(visible);
}
