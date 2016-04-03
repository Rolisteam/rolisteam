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
#include "updaterwindow.h"
#include <QFile>
#include <QMessageBox>
UpdaterWindow::UpdaterWindow(QWidget *parent): QWidget(parent)
{
    //Windows Options
    setFixedSize(258,112);
    setWindowTitle("Updater");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setValue(0);
    reply = m_manager.get(QNetworkRequest(QUrl("http://site.fr/last/application.exe")));
    connect(reply, SIGNAL(finished()), this, SLOT(save()));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgess(qint64, qint64)));
}

void UpdaterWindow::downloadProgess(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal != -1)
    {
        m_progressBar->setRange(0, bytesTotal);
        m_progressBar->setValue(bytesReceived);
    }
}

void UpdaterWindow::save()
{
    reply->deleteLater();
    QFile lastversion("rolisteam.exe");

    if ( lastversion.open(QIODevice::WriteOnly) )
    {
        lastversion.write(reply->readAll());
        lastversion.close();
        QMessageBox::information(this, "", "");
    }
    close();
}
