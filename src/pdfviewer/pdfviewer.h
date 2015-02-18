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

#ifndef PDFVIEWER_H
#define PDFVIEWER_H
#include <poppler-qt4.h>
#include <QLabel>

#include "submdiwindows.h"


class QRubberBand;
class PDFRenderer;
class QScrollArea;
class PDFViewer : public SubMdiWindows
{
public:
    PDFViewer();
    
    virtual void saveFile(const QString & file);
    virtual void openFile(const QString & file);
    //virtual void setRClient(RClient* t);
    
    virtual bool hasDockWidget() const;
    virtual QDockWidget* getDockWidget();
    
    virtual bool defineMenu(QMenu* menu);
    virtual void setCleverURI(CleverURI* uri);
    
    
protected:
    void wheelEvent(QWheelEvent *);
private:
    PDFRenderer* m_render;
    QScrollArea* m_scrollArea;
};

#endif // PDFVIEWER_H
