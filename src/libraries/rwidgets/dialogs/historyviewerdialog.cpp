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
#include "historyviewerdialog.h"
#include "ui_historyviewerdialog.h"
#include <QHeaderView>

HistoryViewerDialog::HistoryViewerDialog(history::HistoryModel* model, QWidget* parent)
    : QDialog(parent), ui(new Ui::HistoryViewerDialog), m_model(model)
{
    ui->setupUi(this);

    ui->m_tableview->setModel(m_model);
    auto header= ui->m_tableview->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
}

HistoryViewerDialog::~HistoryViewerDialog()
{
    delete ui;
}

void HistoryViewerDialog::changeEvent(QEvent* e)
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
