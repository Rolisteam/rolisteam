/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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
#include "tipofdayviewer.h"
#include <QFile>
#include <QMessageBox>
#include <QDesktopServices>

TipOfDayViewer::TipOfDayViewer(QString title, QString msg, QString url,QWidget *parent)
    : QDialog(parent)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("%1 - Tip Of The Day").arg(title));
    m_ui->m_text->setText(msg);
    connect(m_ui->m_neverShowAgain,&QCheckBox::toggled,[&](bool value){
       m_dontshowAgain = value;
    });

    if(!url.isEmpty())
    {
        connect(m_ui->m_buttonBox,&QDialogButtonBox::clicked,[&](QAbstractButton* btn){
            auto standard = m_ui->m_buttonBox->standardButton(btn);
            if(QDialogButtonBox::Open == standard)
            {
                if (!QDesktopServices::openUrl(QUrl(url)))
                {
                    QMessageBox * msgBox = new QMessageBox(
                                QMessageBox::Information,
                                tr("No Service to open links"),
                                tr("Please find the type here: <a href=\"%1\"</a>").arg(url),
                                QMessageBox::Ok
                                );
                    msgBox->exec();
                }
            }
        });
    }
}

bool TipOfDayViewer::dontshowAgain() const
{
    return m_dontshowAgain;
}

void TipOfDayViewer::setDontshowAgain(bool dontshowAgain)
{
    m_dontshowAgain = dontshowAgain;
}


