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
#ifndef DICEALIASMODEL_H
#define DICEALIASMODEL_H

#include <QAbstractListModel>

#include "dicealias.h"
#include <memory>
#include <vector>
/**
 * @brief The DiceAliasModel class
 */
class DiceAliasModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @brief The COLUMN_TYPE enum
     */
    enum COLUMN_TYPE
    {
        PATTERN,
        COMMAND,
        METHOD,
        DISABLE,
        COMMENT
    };
    /**
     * @brief DiceAliasModel
     */
    explicit DiceAliasModel(QObject* parent= nullptr);
    /**
     *
     * */
    ~DiceAliasModel() override;

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
    int columnCount(const QModelIndex& parent) const override;
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
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    /// new methods
    void appendAlias(DiceAlias&& alias);
    void deleteAlias(const QModelIndex& index);

    const std::vector<std::unique_ptr<DiceAlias>>& aliases() const;

    void upAlias(const QModelIndex& index);
    void downAlias(const QModelIndex& index);
    void topAlias(const QModelIndex& index);
    void bottomAlias(const QModelIndex& index);
    void clear();

    void setGM(bool);
    void setModified(bool);

signals:
    void aliasAdded(DiceAlias* alias, int i);
    void aliasRemoved(int i);
    void aliasMoved(int, int);
    void aliasChanged(DiceAlias* alias, int i);

private:
    std::vector<std::unique_ptr<DiceAlias>> m_diceAliasList;
    QStringList m_header;
};

#endif // DICEALIASMODEL_H
