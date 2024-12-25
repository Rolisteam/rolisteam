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
#ifndef CAMPAIGN_CAMPAIGNINTEGRITYDIALOG_H
#define CAMPAIGN_CAMPAIGNINTEGRITYDIALOG_H

#include "data/campaignmanager.h"
#include <QDialog>
#include <QPointer>
#include <QQmlEngine>
#include <memory>

#include "rwidgets_global.h"
namespace Ui
{
class CampaignIntegrityDialog;
}
namespace campaign
{

class RWIDGET_EXPORT CampaignIntegrityController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QStringList missingFiles READ missingFiles NOTIFY missingFilesChanged FINAL)
    Q_PROPERTY(QStringList unmanagedFile READ unmanagedFile NOTIFY unmanagedFileChanged FINAL)
public:
    enum FileAction
    {
        NothingAct= 0,
        RemoveFromCampaign,
        CreateBlank,
        AddToCampaign,
        RemoveFromDisk
    };
    Q_ENUM(FileAction)
    explicit CampaignIntegrityController(QStringList missingFiles, QStringList unmanagedFile, CampaignManager* manager,
                                         QWidget* parent= nullptr);

    QStringList missingFiles() const;
    void setMissingFiles(const QStringList& newMissingFiles);

    QStringList unmanagedFile() const;
    void setUnmanagedFile(const QStringList& newUnmanagedFile);

public slots:
    void performAction(const QString& path, FileAction action);
    void accept();

signals:
    void missingFilesChanged();
    void unmanagedFileChanged();
    void accepted();

private:
    QStringList m_missingFiles;
    QStringList m_unmanagedFile;
    QPointer<CampaignManager> m_manager;
};

class RWIDGET_EXPORT CampaignIntegrityDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CampaignIntegrityDialog(CampaignIntegrityController* ctrl, QWidget *parent= nullptr);
    ~CampaignIntegrityDialog();

protected:
    void changeEvent(QEvent* e);

private:
    Ui::CampaignIntegrityDialog* ui;
    QPointer<CampaignIntegrityController> m_ctrl;
};

} // namespace campaign
#endif // CAMPAIGN_CAMPAIGNINTEGRITYDIALOG_H
