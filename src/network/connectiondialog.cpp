/***************************************************************************
 *     Copyright (C) 2010 by Joseph Boudou                                 *
 *     http://www.rolisteam.org/                                           *
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


#include "connectiondialog.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QProgressBar>

#include "widgets/colorbutton.h"

#define MIN 1
#define MAX 65535

#define WAITING_TIME 1000
/************************
 * ConnectionWaitDialog *
 ************************/



ConnectionWaitDialog::ConnectionWaitDialog()
    : QDialog(), m_socket(NULL)
{

    m_msg << tr("Not connected") << tr("Resolving address") << tr("Connection to Host") <<  tr("Connected");
    setUI();
}

ConnectionWaitDialog::~ConnectionWaitDialog()
{
    delete m_socket;
}

QString ConnectionWaitDialog::getError() const
{
    return m_error;
}

QTcpSocket * ConnectionWaitDialog::connectTo(const QString & host, quint16 port)
{
    m_error = QString();

    if (m_socket == NULL)
    {
        m_socket = new QTcpSocket();
        connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this, SLOT(changeState(QAbstractSocket::SocketState)));
        connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(socketError(QAbstractSocket::SocketError)));
    }

    m_socket->connectToHost(host, port);
    if (m_socket->waitForConnected(WAITING_TIME)) //exec() == QDialog::Accepted)
    {
        QTcpSocket * tmp_socket = m_socket;
        m_socket->disconnect(this);
        m_socket = NULL;
        return tmp_socket;
    }
    else
    {
        return NULL;
    }
}

void ConnectionWaitDialog::setUI()
{
    m_label = new QLabel(m_msg[0]);

    QProgressBar * progress = new QProgressBar();
    progress->setMinimum(0);
    progress->setMaximum(0);
    progress->setTextVisible(false);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Abort);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(canceledConnection()));

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_label);
    mainLayout->addWidget(progress);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

void ConnectionWaitDialog::changeState(QAbstractSocket::SocketState socketState)
{
    m_label->setText(m_msg[socketState]);
    if (socketState == QAbstractSocket::ConnectedState)
    {
        accept();
    }
}

void ConnectionWaitDialog::socketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    m_error = m_socket->errorString();
    reject();
}

void ConnectionWaitDialog::canceledConnection()
{
    if (m_socket != NULL)
        m_socket->abort();
}
