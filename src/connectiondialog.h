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
 * @brief Dialog to set up player's and connection's parameters.
 */
class ConnectionConfigDialog : public QDialog
{
    Q_OBJECT

    public:
        ConnectionConfigDialog();
        ~ConnectionConfigDialog();

        /**
         * @brief set default values.
         * @param name player's name.
         * @param color player's color.
         * @param master set to true if the player is the GM.
         * @param host hostname or address (useless for server).
         * @param port port number.
         * @param server set to true if it connects as server.
         */
        ConnectionConfigDialog(const QString & name, const QColor & color, bool master = false,
                const QString & host = QString(""), quint16 port = 6660, bool server = false);


        QString getName()  const;
        QColor  getColor() const;
        bool    isGM()     const;

        QString getHost()  const;
        quint16 getPort()  const;
        bool    isServer() const;


    private:
        void setUI();

        QLineEdit   * m_name;
        ColorButton * m_color;
        QCheckBox   * m_gm;
        QLabel      * m_hostLabel;
        QLineEdit   * m_host;
        QSpinBox    * m_port;
        QCheckBox   * m_server;

    private slots:
        void changeConnectionType(int state);
};


/**
 * @brief Dialog shown while connectiong a socket to a server.
 * Display a label indicating connection state, a progess bar and an Abort button.
 */
class ConnectionWaitDialog
    : public QDialog
{
    Q_OBJECT

    public:
        ConnectionWaitDialog();
        ~ConnectionWaitDialog();

        /**
         * @brief Return message of the last socket error.
         * Null QString if no error (for example on user's abort).
         */
        QString getError() const;

    public slots:

        /**
         * @brief Exec this dialog with the given parameters.
         * @param host hostname or address to connect to.
         * @param port TCP port to connect to.
         * @return a connected QTcpSocket on success, NULL otherwise.
         */
        QTcpSocket * connectTo(const QString & host, quint16 port);

    private:
        QLabel       * m_label;

        QTcpSocket * m_socket;
        QString      m_error;

        QStringList m_msg;

        void setUI();

    private slots:
        void changeState(QAbstractSocket::SocketState socketState);
        void socketError(QAbstractSocket::SocketError socketError);
        void canceledConnection();
};

#endif
