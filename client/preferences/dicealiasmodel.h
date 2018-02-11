/***************************************************************************
*	Copyright (C) 2009 by Renaud Guezennec                             *
*   http://www.rolisteam.org/contact                   *
*                                                                         *
*   Rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef DICEALIASMODEL_H
#define DICEALIASMODEL_H

#include <QAbstractListModel>

#include "dicealias.h"
#include "network/networkreceiver.h"
#include "preferenceslistener.h"
/**
 * @brief The DiceAliasModel class
 */
class DiceAliasModel : public QAbstractListModel, public PreferencesListener
{
    Q_OBJECT
public:
    /**
     * @brief The COLUMN_TYPE enum
     */
    enum COLUMN_TYPE {PATTERN,VALUE,METHOD,DISABLE,COMMENT};
    /**
     * @brief DiceAliasModel
     */
    DiceAliasModel(QObject* parent);
    /**
      *
      * */
    ~DiceAliasModel();

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const;
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent) const;
    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex &parent) const;
    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role);


    virtual void preferencesHasChanged(QString);


	///new methods
    void setAliases(QList<DiceAlias*>* map);
	void appendAlias();
    QList<DiceAlias*>* getAliases();
    void addAlias(DiceAlias* alias);
    void deleteAlias(QModelIndex& index);
    void upAlias(QModelIndex& index);
    void downAlias(QModelIndex& index);
    void topAlias(QModelIndex& index);
    void moveAlias(int,int);
    void bottomAlias(QModelIndex& index);
    void setGM(bool);
    void clear();
    /**
     * @brief sendOffAllDiceAlias
     */
    void sendOffAllDiceAlias();

    void setModified(bool);

private:
    QList<DiceAlias*>* m_diceAliasList;
    bool m_isGM;
    bool m_modified = false;
    QStringList m_header;
};

#endif // DICEALIASMODEL_H
