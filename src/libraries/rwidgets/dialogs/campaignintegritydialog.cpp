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

// #include "delegates/actiondelegate.h"
//  #include "model/actiononlistmodel.h"

#include <QQmlContext>
#include <QQmlEngine>

namespace campaign
{
CampaignIntegrityDialog::CampaignIntegrityDialog(CampaignIntegrityController* ctrl, QWidget* parent)
    : QDialog(parent), ui(new Ui::CampaignIntegrityDialog), m_ctrl(ctrl)

{
    ui->setupUi(this);
    qmlRegisterSingletonInstance<CampaignIntegrityController>("Integrity", 1, 0, "Controller", m_ctrl);
    QQmlEngine::setObjectOwnership(m_ctrl, QQmlEngine::CppOwnership);

    connect(m_ctrl, &CampaignIntegrityController::accepted, this, &CampaignIntegrityDialog::accept);

    auto engine= ui->quickWidget->engine();
    engine->addImportPath(QStringLiteral("qrc:/qml"));
    engine->addImportPath(QStringLiteral("qrc:/qml/rolistyle"));
    ui->quickWidget->setSource(QUrl("qrc:/qml/Campaign/IntegrityPage.qml"));
    connect(engine, &QQmlEngine::warnings, this,
            [](const QList<QQmlError>& warnings)
            {
                for(auto const& w : warnings)
                    qDebug() << w.toString();
            });
}

CampaignIntegrityDialog::~CampaignIntegrityDialog()
{
    delete ui;
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

////////////////////////////////
/// \brief CampaignIntegrityController::CampaignIntegrityController
/// \param missingFiles
/// \param unmanagedFile
/// \param root
/// \param parent
/////////////////////////////////

CampaignIntegrityController::CampaignIntegrityController(QStringList missingFiles, QStringList unmanagedFile,
                                                         CampaignManager* manager, QWidget* parent)
    : QObject(parent), m_missingFiles(missingFiles), m_unmanagedFile(unmanagedFile), m_manager(manager)
{
    /*, m_missingFileModel(
          new ActionOnListModel(missingFiles, {{"Forget", "edit-delete"}, {"Create", "document-new"}}, root))
    , m_unmanagedFileModel(
        new ActionOnListModel(unmanagedFile, {{"Add into project", "list-add"}, {"Delete", "list-remove"}}, root))*/
}

QStringList CampaignIntegrityController::missingFiles() const
{
    return m_missingFiles;
}

void CampaignIntegrityController::setMissingFiles(const QStringList& newMissingFiles)
{
    if(m_missingFiles == newMissingFiles)
        return;
    m_missingFiles= newMissingFiles;
    emit missingFilesChanged();
}

QStringList CampaignIntegrityController::unmanagedFile() const
{
    return m_unmanagedFile;
}

void CampaignIntegrityController::setUnmanagedFile(const QStringList& newUnmanagedFile)
{
    if(m_unmanagedFile == newUnmanagedFile)
        return;
    m_unmanagedFile= newUnmanagedFile;
    emit unmanagedFileChanged();
}

void CampaignIntegrityController::performAction(const QString& path, FileAction action)
{
    bool res= m_manager->performAction(path, static_cast<Core::CampaignAction>(action));

    if(!res)
        return;

    if(m_unmanagedFile.removeOne(path))
        emit unmanagedFileChanged();

    if(m_missingFiles.removeOne(path))
        emit missingFilesChanged();
}

void CampaignIntegrityController::accept()
{
    emit accepted();
}

} // namespace campaign
