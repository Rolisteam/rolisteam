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


#include "Features.h"
#include "variablesGlobales.h"

/* TODO :
 * create fonctions or classes to read/write QString from/to buffer
 */

/***********
 * Globals *
 ***********/

FeaturesList g_featuresList;


/***********
 * Feature *
 ***********/

Feature::Feature()
    : m_userId(""), m_name(""), m_version(0)
{}

Feature::Feature(const QString & userId, const QString & name, quint8 version)
    : m_userId(userId), m_name(name), m_version(version)
{}

Feature::Feature(const char * buffer)
{   // Same format as send(). (network)

    quint8 tmpSize;

    // Read userId (uuid)
    tmpSize = (quint8) *buffer;
    buffer += sizeof(quint8);
    // QString(QChar *, int) makes a deep copy. No need for memcpy.
    m_userId = QString((const QChar *) buffer, tmpSize);
    buffer += tmpSize * sizeof(QChar);

    // Read name
    tmpSize = (quint8) *buffer;
    buffer += sizeof(quint8);
    m_name = QString((const QChar *) buffer, tmpSize);
    buffer += tmpSize * sizeof(QChar);

    // Read version
    m_version = (quint8) *buffer;
}

Feature::Feature(const Feature & other)
{
    // QString has an operator=, we rely on it.
    m_userId  = other.m_userId;
    m_name    = other.m_name;
    m_version = other.m_version;
}

Feature &
Feature::operator=(const Feature & other)
{
    // Warning : no exception handling. Might be in inconsistent state.
    // QString has an operator=, we rely on it.
    if (this != &other)
    {
        m_userId  = other.m_userId;
        m_name    = other.m_name;
        m_version = other.m_version;
    }
    return *this;
}

bool
Feature::operator==(const Feature & other) const
{
    return ((m_userId == other.m_userId) && (m_name == other.m_name));
}

bool
Feature::isUserId(const QString & userId) const
{
    return (m_userId == userId);
}

bool
Feature::implements(const QString & name, quint8 version) const
{
    return ((m_name == name) && (m_version >= version));
}

quint8
Feature::version() const
{
    return m_version;
}

QString
Feature::toString() const
{
    return QString("%1 -> %2 (%3)").arg(m_userId).arg(m_name).arg(m_version );
}

void
Feature::upgradeTo(quint8 version)
{
    if (m_version < version)
        m_version = version;
}

void
Feature::send(int linkIndex) const
{
    qDebug("Send feature %s to %d", qPrintable(toString()), linkIndex);
    
    int bodySize = 
        // userId (uuid)
        sizeof(quint8) + m_userId.size() * sizeof(QChar) +
        // Name
        sizeof(quint8) + m_name.size() * sizeof(QChar) +
        // Version
        sizeof(quint8);

    char * data = new char[sizeof(enteteMessage) + bodySize];

    // fill header
    enteteMessage * header = (enteteMessage *) data;
    header->categorie     = parametres;
    header->action        = addFeature;
    header->tailleDonnees = bodySize;

    char * p = data + sizeof(enteteMessage);
    quint8 tmpSize;

    // fill userId
    tmpSize = m_userId.size();
    memcpy(p, &tmpSize, sizeof(quint8));
    p += sizeof(quint8);
    memcpy(p, m_userId.data(), tmpSize * sizeof(QChar));
    p += tmpSize * sizeof(QChar);

    // fill name
    tmpSize = m_name.size();
    memcpy(p, &tmpSize, sizeof(quint8));
    p += sizeof(quint8);
    memcpy(p, m_name.data(), tmpSize * sizeof(QChar));
    p += tmpSize * sizeof(QChar);

    // fill version
    memcpy(p, &m_version , sizeof(quint8));

    // send
    emettre(data, sizeof(enteteMessage) + bodySize, linkIndex);

    delete[] data;
}

/****************
 * FeaturesList *
 ****************/

FeaturesList::FeaturesList()
{}

void
FeaturesList::addLocal(const QString & userId)
{
    add(Feature(userId, "Emote", 0));
}

void
FeaturesList::add(const Feature & feature)
{   // In fact, our list is a set ;)
    qDebug("Added feature %s", qPrintable(feature.toString()));

    // If this feature is not is the list for this user, we add it.
    int pos = m_list.indexOf(feature);
    if (pos < 0)
    {
        m_list.append(feature);
        return;
    }
    
    // If the new version is greater than the old one, we change it.
    m_list[pos].upgradeTo(feature.version());
}

int
FeaturesList::countImplemented(const QString & featureName,
        quint8 featureVersion) const
{
    int ret = 0;

    QList<Feature>::const_iterator i;
    for (i = m_list.begin(); i != m_list.end() ; i++)
    {
        if ((*i).implements(featureName, featureVersion))
            ret++;
    }

    return ret;
}

bool
FeaturesList::clientImplements(const Feature & feature) const
{
    int pos = m_list.indexOf(feature);

    if (pos < 0)
        return false;

    return m_list.at(pos).version() >= feature.version();
}

void
FeaturesList::sendThemAll(int linkIndex) const
{
    QList<Feature>::const_iterator i;
    for (i = m_list.begin(); i != m_list.end(); i++)
    {
        (*i).send(linkIndex);
    }
}

void
FeaturesList::delUser(const QString & userId)
{
    QList<Feature>::iterator i;
    for (i = m_list.begin(); i != m_list.end() ;)
    {
        if ((*i).isUserId(userId))
        {
            qDebug("Delete feature %s", qPrintable((*i).toString()));
            i = m_list.erase(i);
        }
        else
        {
            i++;
        }
    }
}
