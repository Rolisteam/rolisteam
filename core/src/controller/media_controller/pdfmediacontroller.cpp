/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "pdfmediacontroller.h"

#include "controller/view_controller/pdfcontroller.h"
#include "worker/messagehelper.h"

PdfMediaController::PdfMediaController(QObject* parent) : MediaManagerBase(Core::ContentType::PDF, parent) {}

PdfMediaController::~PdfMediaController()= default;

bool PdfMediaController::openMedia(const QString& uuid, const std::map<QString, QVariant>& args)
{
    if(uuid.isEmpty() || args.empty())
        return false;

    QByteArray array;
    auto it= args.find(QStringLiteral("data"));
    if(it != args.end())
        array= it->second.toByteArray();

    addPdfController({{"id", uuid}, {"data", array}});
    return true;
}

void PdfMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_pdfs.begin(), m_pdfs.end(),
                            [id](const std::unique_ptr<PdfController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_pdfs.end())
        return;

    (*it)->aboutToClose();
    m_pdfs.erase(it, m_pdfs.end());
}

void PdfMediaController::registerNetworkReceiver() {}

NetWorkReceiver::SendType PdfMediaController::processMessage(NetworkMessageReader* msg)
{
    auto type= NetWorkReceiver::NONE;
    if(nullptr == msg)
        return type;

    if(msg->category() == NetMsg::MediaCategory && msg->action() == NetMsg::AddMedia)
    {
        auto data= MessageHelper::readPdfData(msg);
        addPdfController(data);
    }
    return type;
}

void PdfMediaController::sharePdf(const QString& id)
{
    auto it= std::find_if(m_pdfs.begin(), m_pdfs.end(),
                          [id](const std::unique_ptr<PdfController>& ctrl) { return ctrl->uuid() == id; });

    if(it == m_pdfs.end())
        return;
}

void PdfMediaController::addPdfController(const QHash<QString, QVariant>& params)
{

    auto id= params.value(QStringLiteral("id")).toString();
    auto array= params.value(QStringLiteral("data")).toByteArray();

    std::unique_ptr<PdfController> pdfController(new PdfController(id, array));
    connect(pdfController.get(), &PdfController::sharePdf, this, &PdfMediaController::sharePdf);
    connect(pdfController.get(), &PdfController::openImageAs, this, &PdfMediaController::shareImageAs);

    connect(this, &PdfMediaController::localIsGMChanged, pdfController.get(), &PdfController::setLocalGM);

    emit pdfControllerCreated(pdfController.get());
    m_pdfs.push_back(std::move(pdfController));
}
