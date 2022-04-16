/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "newfiledialog.h"
#include "ui_newfiledialog.h"

NewFileDialog::NewFileDialog(Core::ContentType type, QWidget* parent) : QDialog(parent), ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);

    /*VECTORIALMAP,
        PICTURE,
        NOTES,
        CHARACTERSHEET,
        SHAREDNOTE,
        PDF,
        WEBVIEW,
        INSTANTMESSAGING,
        MINDMAP,
        UNKNOWN*/
    ui->m_type->addItem(tr("Shared Notes"), QVariant::fromValue(Core::ContentType::SHAREDNOTE));
    ui->m_type->addItem(tr("Notes"), QVariant::fromValue(Core::ContentType::NOTES));
    ui->m_type->addItem(tr("MindMap"), QVariant::fromValue(Core::ContentType::MINDMAP));
}

NewFileDialog::~NewFileDialog()
{
    delete ui;
}

QString NewFileDialog::name() const
{
    return ui->m_nameEdit->text();
}

QUrl NewFileDialog::url() const
{
    return QUrl::fromUserInput(ui->m_path->path());
}

Core::ContentType NewFileDialog::type() const
{
    return ui->m_type->currentData().value<Core::ContentType>();
}

void NewFileDialog::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
