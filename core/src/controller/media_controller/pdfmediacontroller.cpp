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

PdfMediaController::PdfMediaController() {}

PdfMediaController::~PdfMediaController()= default;

CleverURI::ContentType PdfMediaController::type() const
{
    return CleverURI::PDF;
}

bool PdfMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    std::unique_ptr<PdfController> pdfCtrl(new PdfController(uri));

    emit pdfControllerCreated(pdfCtrl.get());
    m_pdfs.push_back(std::move(pdfCtrl));
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

NetWorkReceiver::SendType PdfMediaController::processMessage(NetworkMessageReader* msg) {}

void PdfMediaController::setUndoStack(QUndoStack* stack) {}
