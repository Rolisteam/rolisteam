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
#ifndef IMPORTDATAMEDIA_H
#define IMPORTDATAMEDIA_H

#include <QByteArray>
#include <QCoreApplication>
#include <QPointer>
#include <QUndoCommand>

#include "data/campaign.h"
#include "media/mediatype.h"
#include <core_global.h>
namespace commands
{
class CORE_EXPORT ImportDataMedia : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(commands::ImportDataMedia)
public:
    ImportDataMedia(campaign::Campaign* campaign, const QString& name, const QByteArray& data, const QString& destPath);

    void redo() override;
    void undo() override;

    QString destination() const;

private:
    QPointer<campaign::Campaign> m_campaign;
    QString m_uuid;
    QString m_name;
    QString m_tmpPath;
    QString m_destPath;
};
} // namespace commands
#endif // IMPORTDATAMEDIA_H
