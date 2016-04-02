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


#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

#include <QDialog>

#include <QTcpSocket>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>

class ColorButton;

/**
 * @brief Dialog shown while connectiong a socket to a server.
 * Display a label indicating connection state, a progess bar and an Abort button.
 */
class ConnectionWaitDialog
    : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief ConnectionWaitDialog
     */
    ConnectionWaitDialog();
    /**
     * @brief ~ConnectionWaitDialog
     */
    virtual ~ConnectionWaitDialog();

    /**
     * @brief Return message of the last socket error.
     * Null QString if no error (for example on user's abort).
     */
    QString getError() const;

public slots:
    /**
     * @brief changeState
     * @param socketState
     */
    void changeState(QAbstractSocket::SocketState socketState);
    /**
     * @brief socketError
     * @param socketError
     */
    void socketError(QString str);

signals:
    /**
     * @brief canceledConnection
     */
    void canceledConnection();

private:
    /**
     * @brief setUI
     */
    void setUI();

private:
    QLabel       * m_label;
    QTcpSocket * m_socket;
    QString      m_error;
    QStringList m_msg;


};

#endif
