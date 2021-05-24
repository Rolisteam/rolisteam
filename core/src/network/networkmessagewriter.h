/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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

#ifndef NETWORK_MESSAGE_WRITER_H
#define NETWORK_MESSAGE_WRITER_H

#include "network/networkmessage.h"

#include <QDateTime>
#include <QString>

#ifdef QT_GUI_LIB
#include <QPixmap>
#endif

/**
 * @brief The NetworkMessageWriter class
 */
class NetworkMessageWriter : public NetworkMessage
{
public:
    NetworkMessageWriter(NetMsg::Category categorie, NetMsg::Action action,
                         NetworkMessage::RecipientMode mode= NetworkMessage::All, int size= 128);
    virtual ~NetworkMessageWriter() override;

    NetMsg::Category category() const override;
    NetMsg::Action action() const override;

    void reset();

    void uint8(quint8 data);
    void uint16(quint16 data);
    void uint32(quint32 data);
    void uint64(quint64 data);

    void string8(const QString& data);
    void string16(const QString& data);
    void string32(const QString& data);

    void dateTime(const QDateTime& time);

    void byteArray32(const QByteArray& data);

#ifdef QT_GUI_LIB
    bool pixmap(const QPixmap& pix);
#endif
    void rgb(unsigned int color);

    quint32 getDataSize() const;

    void int8(qint8 data);
    void int16(qint16 data);
    void int32(qint32 data);
    void int64(qint64 data);

    void real(qreal data);

    /**
     * @brief getRecipientList
     * @return
     */
    void setRecipientList(QStringList, NetworkMessage::RecipientMode mode);
    virtual QStringList getRecipientList() const override;
    NetworkMessage::RecipientMode getRecipientMode() const override;

    /**
     * @brief getData - for testing only
     * @return  data as QByteArray
     */
    QByteArray getData();

protected:
    NetworkMessageHeader* buffer() const override;

private:
    NetworkMessageHeader* m_header;
    char* m_buffer;
    char* m_begin;
    char* m_currentPos;
    char* m_end;

    void string(const QString& data, int sizeQChar);
    void makeRoom(int size);
    long long int m_sizeBuffer;
    long long int m_sizeData;
    QStringList m_recipientList;
    NetworkMessage::RecipientMode m_mode;
};

#endif
