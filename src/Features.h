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


#ifndef FEATURES_H
#define FEATURES_H

#include <QList>
#include <QString>
#include <QObject>

class DataReader;

/**
 * @brief Feature represents a feature of a client software.
 *
 * Features are designated by a name and a version. A feature of version n includes all versions with same name and
 * version inferior or equal to n.
 */
class Feature {

    public:

        // Constructors

        Feature();
        Feature(const Feature & other);

        /**
         * @brief Constructor.
         *
         * @param userId unique identifier of the player this feature belongs to.
         * @param name name of this feature.
         * @param version version of this feature.
         */
        Feature(const QString & userId, const QString & name, quint8 version);

        /**
         * @brief Constructor from network data.
         *
         * Made a new Feature out of a network message.
         *
         * @param data.
         */
        Feature(DataReader & data); // read the data part of a message

        // Operators

        Feature & operator=(const Feature & other);

        /**
         * @brief Equality operator.
         *
         * Two features are equals if they belong to the same user and have the same name. Version isn't taken into
         * account. That means that two features might be equals even if they don't have the same version number.
         *
         * @param other the Feature you want to compare this one to.
         */
        bool operator==(const Feature & other) const; // don't care about version

        // Getters

        /**
         * @brief Test if this feature belongs to a player.
         *
         * @param userId unique identifier of the player.
         * @return true only if this feature belongs to the user designated by \a userId.
         */
        bool   isUserId(const QString & userId) const;

        /**
         * @brief Test if this feature implies the feature's name and version provided.
         *
         * @param name name of the feature we want.
         * @param version version of the feature we want.
         * @return true only if feature's name is \a name and feature's version is superior or equal to \a version.
         */
        bool   implements(const QString & name, quint8 version) const;

        quint8 version() const;
        QString toString() const;

        // setters

        /**
         * @brief Change version number, but only to an greater value.
         *
         * @param version version number to upgrade this feature to.
         */
        void   upgradeTo(quint8 version);

        // pseudo-slots

        /**
         * @brief Send this feature as a network message.
         *
         * @param linkIndex index of the network link in the global links array.
         */
        void send(int linkIndex) const;

    private:
        QString m_userId;
        QString m_name;
        quint8  m_version;
};

/**
 * @brief FeatureList maintains a features database.
 */
class FeaturesList
 : public QObject
{
    Q_OBJECT

    public:

        // Constructors

        /**
         * @brief Default constructor.
         *
         * Create an empty database.
         */
        FeaturesList(QObject * parent = NULL);

        // Getters

        /**
         * @brief Count how many entries implements a feature.
         *
         * Use the semantic of Feature::implements().
         *
         * @param featureName name of the feature we are looking for.
         * @param featureVersion version of the feature we are looking for.
         * @return the number of features in the database for which Feature::implements returns \c true.
         */
        int  countImplemented(const QString & featureName, quint8 featureVersion) const;

        /**
         * @brief Find if a Feature is in the database.
         *
         * Find a Feature in the database wich :
         * @li has the same owner ;
         * @li has the same name ;
         * @li has a version number greater or equal.
         *
         * @param feature the feature we're looking for.
         * @return true only if a corresponding Feature is found in the database.
         */
        bool clientImplements(const Feature & feature) const;

        // Setters

        /**
         * @brief Add local client features to the database.
         *
         * @param userId unique identifier of the local player.
         */
        void addLocal(const QString & userId);

        /**
         * @brief Add a Feature to the database.
         *
         * @param feature feature to add.
         */
        void add(const Feature & feature);

        // pseudo-slots

        /**
         * @brief Send all the features in the database to the network.
         *
         * @param linkIndex index of the network link in the global links array.
         */
        void sendThemAll(int linkIndex = 0) const;

        /**
         * @brief Delete all entries in the database wich refers to a player.
         *
         * @param userId unique identifier of the player.
         */
        void delUser(const QString & userId);

        // Event handler
        virtual bool event(QEvent * event);

    private:
        QList<Feature> m_list;
};

#endif
