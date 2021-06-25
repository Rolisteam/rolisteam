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
#ifndef CAMPAIGNEDITOR_H
#define CAMPAIGNEDITOR_H

#include <QObject>
#include <QUndoCommand>

#include <memory>

#include "media/mediatype.h"

namespace campaign
{
class Campaign;
class Media;
class CampaignEditor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Campaign* campaign READ campaign NOTIFY campaignChanged)
public:
    explicit CampaignEditor(QObject* parent= nullptr);

    Campaign* campaign() const;

    void createNew(const QString& dir);
    bool open(const QString& from, bool discard);
    bool save(const QString& to);
    bool saveCopy(const QString& src, const QString& to);

    // media
    bool addMedia(const QString& src, const QByteArray& array);
    bool removeMedia(const QString& src);

    // character
    // QString addFileIntoCharacters(const QString& src);
    // bool removeFileFromCharacters(const QString& path);

    QString saveAvatar(const QString& id, const QByteArray& array);

    QString mediaFullPath(const QString& file, Core::ContentType type);
    void doCommand(QUndoCommand* command);

    QString campaignDir() const;
    QString currentDir() const;

signals:
    void campaignChanged();
    void performCommand(QUndoCommand* command);
    void importedFile(campaign::Media* media);

private:
    QString m_root;
    std::unique_ptr<campaign::Campaign> m_campaign;
};
} // namespace campaign
#endif // CAMPAIGNEDITOR_H
