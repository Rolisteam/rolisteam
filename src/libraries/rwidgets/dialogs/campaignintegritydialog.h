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

#include <QDialog>
#include <memory>

#include "model/actiononlistmodel.h"
#include "rwidgets_global.h"
namespace Ui
{
class CampaignIntegrityDialog;
}
class ActionOnListModel;
namespace campaign
{

class RWIDGET_EXPORT CampaignIntegrityDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool canValidate READ canValidate NOTIFY canValidateChanged)
public:
    explicit CampaignIntegrityDialog(QStringList missingFiles, QStringList unmanagedFile, const QString& root,
                                     QWidget* parent= nullptr);
    ~CampaignIntegrityDialog();

    bool canValidate() const;

    const QList<DataInfo>& missingFileActions() const;
    const QList<DataInfo>& unmanagedFileActions() const;

public slots:
    void validate();
    void refuse();
    void setAction(int modelId, int index, int actionId);

protected:
    void changeEvent(QEvent* e);

signals:
    void canValidateChanged();

private:
    Ui::CampaignIntegrityDialog* ui;
    std::unique_ptr<ActionOnListModel> m_missingFileModel;
    std::unique_ptr<ActionOnListModel> m_unmanagedFileModel;
};

} // namespace campaign
#endif // CAMPAIGN_CAMPAIGNINTEGRITYDIALOG_H
