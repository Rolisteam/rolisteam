/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#include <QtGui>


#include "pdfviewer.h"
#include "pdfrenderer.h"

PDFViewer::PDFViewer()
{
    setWindowIcon(QIcon(CleverURI::getIcon(CleverURI::PDF)));
    m_render = new PDFRenderer();
    m_scrollArea = new QScrollArea(this);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    setSizePolicy(sizePolicy);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setWidget(m_render);
    setWidget(m_scrollArea);

}
void PDFViewer::saveFile(const QString & file)
{

}

void PDFViewer::openFile(const QString & file)
{
    m_render->loadDocument(file);
    m_render->showPage(m_render->getCurrentPage());

}
bool PDFViewer::defineMenu(QMenu* menu)
{
    return false;
}

void PDFViewer::setCleverURI(CleverURI* uri)
{
    openFile(uri->getUri());
    SubMdiWindows::setCleverURI(uri);
}
bool PDFViewer::hasDockWidget() const
{
    return false;
}
QDockWidget* PDFViewer::getDockWidget()
{
    return NULL;
}
void PDFViewer::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier)
    {
      qreal scale = m_render->getScaleFactor() + event->delta() / 120;
      m_render->setScaleFactor(scale);
    }
    else
        SubMdiWindows::wheelEvent(event);
}

