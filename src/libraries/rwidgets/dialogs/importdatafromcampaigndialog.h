/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef IMPORTDATAFROMCAMPAIGNDIALOG_H
#define IMPORTDATAFROMCAMPAIGNDIALOG_H

#include <QDialog>

#include "core/media/mediatype.h"

namespace Ui
{
class ImportDataFromCampaignDialog;
}

class ImportDataFromCampaignDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool validSource READ validSource NOTIFY sourceChanged)
    Q_PROPERTY(QString source READ source NOTIFY sourceChanged)
    Q_PROPERTY(QVector<Core::CampaignDataCategory> categoryList READ categoryList CONSTANT)

public:
    explicit ImportDataFromCampaignDialog(const QString& campaignRoot, QWidget* parent= nullptr);
    ~ImportDataFromCampaignDialog();

    bool validSource() const;

    const QString source() const;

    const QVector<Core::CampaignDataCategory> categoryList() const;

signals:
    void sourceChanged();

private slots:
    void setSource(const QString& newSource);

protected:
    void changeEvent(QEvent* e);

private:
    Ui::ImportDataFromCampaignDialog* ui;
    QVector<Core::CampaignDataCategory> m_categoryList;
    QString m_root;
};

#endif // IMPORTDATAFROMCAMPAIGNDIALOG_H
