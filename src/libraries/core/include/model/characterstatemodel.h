/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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
#include <core_global.h>
#include <memory>
#include <vector>
/**
 * @brief The CharacterStateModel class
 */
class CORE_EXPORT CharacterStateModel : public QAbstractListModel //, public PreferencesListener, public NetWorkReceiver
{
    Q_OBJECT
public:
    /**
     * @brief The COLUMN_TYPE enum
     */
    enum COLUMN_TYPE
    {
        LABEL= Qt::UserRole + 1,
        COLOR,
        PICTUREPATH,
        PICTURE,
        ID
    };
    /**
     * @brief CharacterStateModel
     */
    explicit CharacterStateModel(QObject* parent= nullptr);
    /**
     *
     * */
    ~CharacterStateModel() override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index, int role) const override;
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex& idx, const QVariant& value, int role) override;

    /// new methods
    const std::vector<std::unique_ptr<CharacterState>>& statesList() const;

    void appendState(CharacterState&& state);
    void deleteState(const QModelIndex& index);
    void clear();

    void upState(const QModelIndex& index);
    void downState(const QModelIndex& index);
    void topState(const QModelIndex& index);
    void bottomState(const QModelIndex& index);
    void moveState(int from, int to);
    void removeStateAt(int i);

signals:
    void characterStateAdded(CharacterState* state, int i);
    void stateRemoved(const QString& id);
    void stateMoved(int from, int to);
    void stateChanged();

private:
    std::vector<std::unique_ptr<CharacterState>> m_stateList;
    QStringList m_header;
};

#endif // CHARACTERSTATEMODEL_H
