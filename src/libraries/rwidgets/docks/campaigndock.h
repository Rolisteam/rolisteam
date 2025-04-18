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
#ifndef RWIDGET_CAMPAIGN_DOCK_H
#define RWIDGET_CAMPAIGN_DOCK_H

#include "controller/preferencescontroller.h"
#include "media/mediatype.h"
#include "model/mediafilteredmodel.h"
#include "model/mediamodel.h"
#include "rwidgets/modelviews/campaignview.h"
#include "rwidgets_global.h"

#include <QDockWidget>
#include <QPointer>
#include <QSettings>
#include <memory>

namespace Ui
{
class CampaignDock;
}

namespace campaign
{
class CampaignEditor;
/**
 * @brief SessionManager is a dockwidget which displays all loaded resources in the current session. It provides
 * shortcut to open them. It is part of the MVC architecture. It manages the view and the model.
 */
class RWIDGET_EXPORT CampaignDock : public QWidget
{
    Q_OBJECT
public:
    explicit CampaignDock(CampaignEditor* campaign, PreferencesController* prefs, QWidget* parent= nullptr);
    virtual ~CampaignDock();

public slots:
    void setCampaign(CampaignEditor* campaign);
    void openExternally(const QString& id, const QString& path, Core::ContentType type);

signals:
    void campaignChanged();
    void openResource(const QString& id, const QString& path, Core::ContentType type);
    void removeFile(const QString& path);

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::CampaignDock* m_ui;
    QPointer<CampaignEditor> m_campaignEditor;
    QPointer<PreferencesController> m_preferences;
    std::unique_ptr<MediaModel> m_model;
    std::unique_ptr<MediaFilteredModel> m_filteredModel;
};
} // namespace campaign
#endif // RWIDGET_CAMPAIGN_DOCK_H
