/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     http://www.rolisteam.org/                                         *
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

#ifndef DATA_READER_H
#define DATA_READER_H

#include "network/networkmessage.h"
#include <QString>
/**
 * @brief The NetworkMessageReader class
 */
class NetworkMessageReader : public NetworkMessage
{
public:
    NetworkMessageReader();
    NetworkMessageReader(const NetworkMessageHeader& header, const char* buffer);
    NetworkMessageReader(const NetworkMessageReader& other);
    virtual ~NetworkMessageReader();

    bool isValid();

    NetMsg::Category category() const;
    NetMsg::Action action() const;

    void reset();

    void setData(const QByteArray& bytes);

    size_t left() const;

    quint8 uint8();
    quint16 uint16();
    quint32 uint32();
    quint64 uint64();

    QString string8();
    QString string16();
    QString string32();

    QByteArray byteArray32();

    QString string(quint64 size);
    unsigned int rgb();
    qreal real();
    qint8 int8();
    qint16 int16();
    qint32 int32();
    qint64 int64();

    NetworkMessageHeader* header() const;
    void setHeader(NetworkMessageHeader* header);

    void resetToData();
    /////////////////////////////////
    ///
    /// \brief readRecipient
    ///
    /////////////////////////////////
    void readRecipient();

    NetworkMessage::RecipientMode getRecipientMode() const;

    virtual QStringList getRecipientList() const;

    void setInternalData(const QByteArray& bytes);

protected:
    NetworkMessageHeader* buffer();
    bool isSizeReadable(size_t size);

private:
    NetworkMessageHeader* m_header= nullptr;
    bool m_outMemory= false;
    char* m_buffer= nullptr;
    const char* m_pos= nullptr;
    const char* m_end= nullptr;
    RecipientMode m_mode;
    QStringList m_recipientList;
};

#endif
