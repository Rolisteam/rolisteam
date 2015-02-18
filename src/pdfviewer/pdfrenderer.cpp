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


#include "pdfrenderer.h"

PDFRenderer::PDFRenderer()
    : m_pdf(NULL),rubberBand(NULL)
{
    m_currentPage = 0;
    m_scaleFactor = 0.5;
    setAlignment(Qt::AlignCenter);

}


void PDFRenderer::showPage(int page)
{

    QImage image = m_pdf->page(page)->renderToImage(
                        m_scaleFactor * physicalDpiX(),
                        m_scaleFactor * physicalDpiY());

    setPixmap(QPixmap::fromImage(image));

    qDebug() << "taill page " <<rect() << image.rect();
}

void PDFRenderer::loadDocument(QString filename)
{
    m_pdf = Poppler::Document::load(filename);
    if (m_pdf) {
        m_pdf->setRenderHint(Poppler::Document::Antialiasing);
        m_pdf->setRenderHint(Poppler::Document::TextAntialiasing);
    }

}
quint32 PDFRenderer::getCurrentPage() const
{
    return m_currentPage;
}

void PDFRenderer::setCurrentPage(quint32 currentpage)
{
    m_currentPage = currentpage;
}

void PDFRenderer::mousePressEvent(QMouseEvent *event)
{
    if (!m_pdf)
        return;

    dragPosition = event->pos();
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rubberBand->setGeometry(QRect(dragPosition, QSize()));
    rubberBand->show();
}

void PDFRenderer::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_pdf)
        return;
if (rubberBand)
    rubberBand->setGeometry(QRect(dragPosition, event->pos()).normalized());
}

void PDFRenderer::mouseReleaseEvent(QMouseEvent *)
{
    if (!m_pdf)
        return;
    if (!rubberBand)
      return;
    if (!rubberBand->size().isEmpty()) {
        // Correct for the margin around the image in the label.
        QRectF rect = QRectF(rubberBand->pos(), rubberBand->size());
        rect.moveLeft(rect.left() - (width() - pixmap()->width()) / 2.0);
        rect.moveTop(rect.top() - (height() - pixmap()->height()) / 2.0);
        selectedText(rect);
    }

    rubberBand->hide();
}
QMatrix PDFRenderer::matrix() const
{
    return QMatrix(m_scaleFactor * physicalDpiX() / 72.0, 0,
                   0, m_scaleFactor * physicalDpiY() / 72.0,
                   0, 0);
}
void PDFRenderer::selectedText(const QRectF &rect)
{
    QRectF selectedRect = matrix().inverted().mapRect(rect);
    // QString text = doc->page(currentPage)->text(selectedRect);

    QString text;
    bool hadSpace = false;
    QPointF center;
    foreach (Poppler::TextBox *box, m_pdf->page(m_currentPage)->textList()) {
        if (selectedRect.intersects(box->boundingBox())) {
            if (hadSpace)
                text += " ";
            if (!text.isEmpty() && box->boundingBox().top() > center.y())
                text += "\n";
            text += box->text();
            hadSpace = box->hasSpaceAfter();
            center = box->boundingBox().center();
        }
    }

    if (!text.isEmpty())
        emit textSelected(text);
}
void PDFRenderer::setScaleFactor(qreal factor)
{
    m_scaleFactor = factor;
}
qreal PDFRenderer::getScaleFactor() const
{
    return m_scaleFactor;
}
