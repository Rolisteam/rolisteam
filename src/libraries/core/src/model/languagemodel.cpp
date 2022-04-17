/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "model/languagemodel.h"

#include "worker/iohelper.h"

#include <QJsonArray>
#include <QJsonObject>

void fetchData(QList<LanguageInfo>& infos)
{
    auto data= IOHelper::fetchLanguageModel();

    for(auto langRef : data)
    {
        auto lang= langRef.toObject();
        LanguageInfo info;
        info.code= lang["code"].toString();
        info.commonLanguageName= lang["commonName"].toString();
        info.languageName= lang["langname"].toString();
        QStringList list;
        auto paths= lang["path"].toArray();
        for(auto const& path : qAsConst(paths))
        {
            list << path.toString();
        }
        info.path= list;

        infos.append(info);
    }
}

LanguageModel::LanguageModel(QObject* parent) : QAbstractListModel(parent)
{

    fetchData(m_languageInfoList);
}

int LanguageModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_languageInfoList.size();
}

QVariant LanguageModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole)
        return {};

    auto lang= m_languageInfoList[index.row()];

    return QString("%1 - %2").arg(lang.languageName, lang.commonLanguageName);
}

QStringList LanguageModel::pathFromIndex(const QModelIndex& index)
{
    if(!index.isValid())
        return {};

    auto lang= m_languageInfoList[index.row()];

    return lang.path;
}

int LanguageModel::indexSystemLocale(const QString& localeCode)
{

    int index= -1;
    auto it= std::find_if(std::begin(m_languageInfoList), std::end(m_languageInfoList),
                          [localeCode](const LanguageInfo& info) { return info.code == localeCode; });

    if(it == std::end(m_languageInfoList))
    {
        auto it2= std::find_if(std::begin(m_languageInfoList), std::end(m_languageInfoList),
                               [localeCode](const LanguageInfo& info)
                               {
                                   auto lang= localeCode.left(2);
                                   return info.code.startsWith(lang);
                               });

        if(it != std::end(m_languageInfoList))
            index= std::distance(std::begin(m_languageInfoList), it2);
    }
    else
        index= std::distance(std::begin(m_languageInfoList), it);

    return index;
}
