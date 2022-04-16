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
    Q_PROPERTY(Campaign* campaign READ campaign CONSTANT)
public:
    explicit CampaignEditor(QObject* parent= nullptr);

    Campaign* campaign() const;

    bool open(const QString& from);

    // media
    bool addMedia(const QString& id, const QString& dest, const QByteArray& array);
    bool removeMedia(const QString& src);
    bool removeFile(const QString& src);
    bool createDefault(const QString& src);

    QString saveAvatar(const QString& id, const QByteArray& array);

    QString mediaFullPathWithExtension(const QString& file, Core::ContentType type) const;
    QString mediaFullPath(const QString& file) const;

    void doCommand(QUndoCommand* command);

    QString campaignDir() const;
    QString currentDir() const;

    bool mergeAudioFile(const QString& source, const QString& dest);
    bool copyMedia(const QString& source, const QString& dest, Core::MediaType type);
    bool copyTheme(const QString& source, const QString& dest);
    bool mergeJsonArrayFile(const QString& source, const QString& dest);

    bool loadDiceAlias(const QString& source); // import
    bool loadStates(const QString& source, const QString& srcDir, const QString& dest,
                    const QString& destDir); // import
    bool loadNpcData(const QString& source, const QString& srcDir, const QString& dest,
                     const QString& destDir); // import

signals:
    void campaignLoaded(const QStringList missingFiles, const QStringList unmanagedFiles);
    void performCommand(QUndoCommand* command);
    void importedFile(campaign::Media* media);

private:
    QString m_root;
    std::unique_ptr<campaign::Campaign> m_campaign;
};
} // namespace campaign
#endif // CAMPAIGNEDITOR_H
