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

#ifndef PDFRENDERER_H
#define PDFRENDERER_H
#include <poppler-qt4.h>
#include <QLabel>

class QRubberBand;
class PDFRenderer : public QLabel
{
    
    Q_OBJECT
public:
    PDFRenderer();
    
    
    void showPage(int page);
    void loadDocument(QString filename);
    void setScaleFactor(qreal factor);
    
    quint32 getCurrentPage() const;
    void setCurrentPage(quint32 currentpage);
    QMatrix matrix() const;
    qreal getScaleFactor() const;
    
signals:
    void textSelected(QString str);
    
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *);
    
private:
    void selectedText(const QRectF &rect);
    void nextPage();
    void previousPage();
private:
    Poppler::Document* m_pdf;
    QRubberBand *rubberBand;
    QPoint dragPosition;
    qint32 m_currentPage;
    qreal m_scaleFactor;
    
};


#endif // PDFRENDERER_H
