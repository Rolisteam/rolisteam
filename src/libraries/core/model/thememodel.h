/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QAbstractListModel>
#include <memory>

class RolisteamTheme;
class ThemeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum customRole
    {
        Name= Qt::UserRole + 1,
        Theme
    };
    explicit ThemeModel(QObject* parent= nullptr);
    ~ThemeModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    void removeTheme(int pos);
    QString name(int themePos) const;
    void addTheme(RolisteamTheme* theme);
    const std::vector<std::unique_ptr<RolisteamTheme>>& themes() const;
    RolisteamTheme* theme(int pos) const;

private:
    std::vector<std::unique_ptr<RolisteamTheme>> m_themeList;
};

#endif // THEMEMODEL_H
