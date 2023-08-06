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
#include "campaignintegritydialog.h"
#include "ui_campaignintegritydialog.h"

#include "delegates/actiondelegate.h"
#include "model/actiononlistmodel.h"

#include <QQmlContext>
#include <QQmlEngine>

namespace campaign
{
CampaignIntegrityDialog::CampaignIntegrityDialog(QStringList missingFiles, QStringList unmanagedFile,
                                                 const QString& root, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::CampaignIntegrityDialog)
    , m_missingFileModel(
          new ActionOnListModel(missingFiles, {{"Forget", "edit-delete"}, {"Create", "document-new"}}, root))
    , m_unmanagedFileModel(
          new ActionOnListModel(unmanagedFile, {{"Add into project", "list-add"}, {"Delete", "list-remove"}}, root))
{
    ui->setupUi(this);
    auto engine= ui->quickWidget->engine();
    engine->addImportPath(QStringLiteral("qrc:/qml"));
    engine->addImportPath(QStringLiteral("qrc:/qml/rolistyle"));
    engine->rootContext()->setContextProperty("_dialog", this);
    engine->rootContext()->setContextProperty("_missingFilesModel", m_missingFileModel.get());
    engine->rootContext()->setContextProperty("_unmanagedFilesModel", m_unmanagedFileModel.get());
    ui->quickWidget->setSource(QUrl("qrc:/qml/Campaign/IntegrityPage.qml"));
    connect(engine, &QQmlEngine::warnings, this, [](const QList<QQmlError>& warnings)
    {
        for(auto const& w : warnings)
            qDebug() << w.toString();
    });
}

CampaignIntegrityDialog::~CampaignIntegrityDialog()
{
    delete ui;
}

bool CampaignIntegrityDialog::canValidate() const
{
    return m_missingFileModel->canValidate() && m_unmanagedFileModel->canValidate();
}

const QList<DataInfo>& CampaignIntegrityDialog::missingFileActions() const
{
    return m_missingFileModel->dataset();
}

const QList<DataInfo>& CampaignIntegrityDialog::unmanagedFileActions() const
{
    return m_unmanagedFileModel->dataset();
}

void CampaignIntegrityDialog::validate()
{
    accept();
}

void CampaignIntegrityDialog::refuse()
{
    reject();
}

void CampaignIntegrityDialog::setAction(int modelId, int index, int actionId)
{
    auto model= (modelId == 0) ? m_missingFileModel.get() : m_unmanagedFileModel.get();

    model->setAction(index, actionId);
    emit canValidateChanged();
}

void CampaignIntegrityDialog::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
} // namespace campaign
