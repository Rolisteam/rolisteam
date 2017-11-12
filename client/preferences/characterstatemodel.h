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
#ifndef CHARACTERSTATEMODEL_H
#define CHARACTERSTATEMODEL_H

#include <QAbstractListModel>

#include "data/characterstate.h"
#include "network/networkreceiver.h"
#include "preferenceslistener.h"
/**
 * @brief The CharacterStateModel class
 */
class CharacterStateModel : public QAbstractListModel, public PreferencesListener, public NetWorkReceiver
{
    Q_OBJECT
public:
    /**
     * @brief The COLUMN_TYPE enum
     */
    enum COLUMN_TYPE {LABEL,COLOR,PICTURE};
    /**
     * @brief CharacterStateModel
     */
    CharacterStateModel();
    /**
      *
      * */
    ~CharacterStateModel();

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

    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg, NetworkLink* link = nullptr);


	///new methods
	void setStates(QList<CharacterState*>* map);
	void appendState();

	QList<CharacterState*>* getCharacterStates();
	void addState(CharacterState* state);
	void deleteState(QModelIndex& index);
	void upState(QModelIndex& index);
	void downState(QModelIndex& index);
	void topState(QModelIndex& index);
	void moveState(int,int);
	void bottomState(QModelIndex& index);
    void setGM(bool);
    void clear();
    /**
	 * @brief sendOffAllCharacterState
     */
	void sendOffAllCharacterState(NetworkLink*);

    void processAddState(NetworkMessageReader *msg);
    void processMoveState(NetworkMessageReader *msg);
    void processRemoveState(NetworkMessageReader *msg);
private:
    QList<CharacterState*>* m_stateList;
    QList<CharacterState*>* m_stateListFromGM;
    bool m_isGM;
    QStringList m_header;
};

#endif // CHARACTERSTATEMODEL_H
