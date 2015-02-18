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


PDFViewer::PDFViewer()
{
    m_label = new QLabel();
    setWidget(m_label);
    rubberBand = NULL;
}
void PDFViewer::saveFile(const QString & file)
{

}

void PDFViewer::openFile(const QString & file)
{
    m_pdf = Poppler::Document::load(file);
    showPage(0);
}
void PDFViewer::showPage(int page)
{
    QImage image = m_pdf->page(page)->renderToImage(
                        1 * physicalDpiX(),
                        1 * physicalDpiY());

    m_label->setPixmap(QPixmap::fromImage(image));
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

void PDFViewer::mousePressEvent(QMouseEvent *event)
{
    if (!m_pdf)
        return;

    dragPosition = event->pos();
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rubberBand->setGeometry(QRect(dragPosition, QSize()));
    rubberBand->show();
}

void PDFViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_pdf)
        return;
if (rubberBand)
    rubberBand->setGeometry(QRect(dragPosition, event->pos()).normalized());
}

void PDFViewer::mouseReleaseEvent(QMouseEvent *)
{
    if (!m_pdf)
        return;
    if (!rubberBand)
      return;
    if (!rubberBand->size().isEmpty()) {
        // Correct for the margin around the image in the label.
        QRectF rect = QRectF(rubberBand->pos(), rubberBand->size());
        rect.moveLeft(rect.left() - (width() - m_label->pixmap()->width()) / 2.0);
        rect.moveTop(rect.top() - (height() - m_label->pixmap()->height()) / 2.0);
        //m_label->selectedText(rect);
    }

    rubberBand->hide();
}
