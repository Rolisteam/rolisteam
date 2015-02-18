/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			                   *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

#include "WorkspaceAmeliore.h"
#include "constantesGlobales.h"

#include <QPixmap>


ImprovedWorkspace::ImprovedWorkspace(QWidget *parent)
: QMdiArea(parent)
{
    initCursors();
    m_variableSizeBackground = new QPixmap(this->viewport()->size());
    m_variableSizeBackground->fill(Qt::darkGray);
    QPainter painter(m_variableSizeBackground);

    m_backgroundPicture = new QPixmap(":/resources/icones/fond workspace macos.bmp");

    painter.drawPixmap(0,0,m_backgroundPicture->width(),m_backgroundPicture->height(),*m_backgroundPicture);
    this->setBackground(QBrush(*m_variableSizeBackground));
    m_currentCursor = m_handCursor;

}


void ImprovedWorkspace::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
    delete m_variableSizeBackground;

    m_variableSizeBackground = new QPixmap(this->viewport()->size());
    m_variableSizeBackground->fill(Qt::darkGray);
    QPainter painter(m_variableSizeBackground);

    painter.drawPixmap(0,0,m_backgroundPicture->width(),m_backgroundPicture->height(),*m_backgroundPicture);
    this->setBackground(QBrush(*m_variableSizeBackground));


}
void ImprovedWorkspace::currentToolChanged(ToolsBar::SelectableTool tool)
{
    //enum SelectableTool {PEN, LINE, EMPTYRECT, FILLRECT, EMPTYELLIPSE, FILLEDELLIPSE, TEXT, HANDLER, ADDNPC, DELNPC, MOVECHARACTER, STATECHARACTER};

    switch(tool)
    {
        case ToolsBar::PEN:
        case ToolsBar::LINE:
        case ToolsBar::EMPTYRECT:
        case ToolsBar::FILLRECT:
        case ToolsBar::EMPTYELLIPSE:
        case ToolsBar::FILLEDELLIPSE:
            m_currentCursor = m_drawingCursor;
            break;
        case ToolsBar::TEXT:
            m_currentCursor = m_textCursor;
            break;
        case ToolsBar::HANDLER:
            m_currentCursor = m_handCursor;
            break;
        case ToolsBar::ADDNPC:
            m_currentCursor = m_addCursor;
            break;
        case ToolsBar::DELNPC:
            m_currentCursor = m_deleteCursor;
            break;
        case ToolsBar::MOVECHARACTER:
            m_currentCursor = m_moveCursor;
            break;
        case ToolsBar::STATECHARACTER:
            m_currentCursor = m_stateCursor;
            break;
    }

    emit currentCursorChanged(m_currentCursor);
    emit currentToolHasChanged(tool);
}

void ImprovedWorkspace::initCursors()
{
        QBitmap bitmap(":/resources/icones/pointeur dessin.png");
        #ifdef Q_WS_MAC
            QBitmap drawmask(32,32);
            drawmask.fill(Qt::color0);
            m_drawingCursor = new QCursor(bitmap,drawmask, 8, 8);
        #else
            m_drawingCursor = new QCursor(bitmap, 8, 8);
        #endif

            QBitmap textBitmap(":/resources/icones/pointeur texte.png");
        #ifdef Q_WS_MAC
            QBitmap drawmask(32,32);
            drawmask.fill(Qt::color0);
            m_textCursor = new QCursor(textBitmap, drawmask, 4, 13);
        #else
            m_textCursor = new QCursor(textBitmap/*, masqueTexte*/, 4, 13);
        #endif


        QPixmap movePixmap(":/resources/icones/pointeur deplacer.png");
        m_moveCursor = new QCursor(movePixmap, 0, 0);

        QPixmap rotatePixmap(":/resources/icones/pointeur orienter.png");
        m_rotateCursor = new QCursor(rotatePixmap, 10, 12);

        QPixmap pipettePixmap(":/resources/icones/pointeur pipette.png");
        m_pipetteCursor = new QCursor(pipettePixmap, 1, 19);

        QPixmap addPixmap(":/resources/icones/pointeur ajouter.png");
        m_addCursor = new QCursor(addPixmap, 6, 0);

        QPixmap deletePixmap(":/resources/icones/pointeur supprimer.png");
        m_deleteCursor = new QCursor(deletePixmap, 6, 0);

        QPixmap StatePixmap(":/resources/icones/pointeur etat.png");
        m_stateCursor = new QCursor(StatePixmap, 0, 0);

        m_handCursor = new QCursor(Qt::OpenHandCursor);

}

void ImprovedWorkspace::addWidget(SubMdiWindows* subWindow)
{
    addSubWindow(subWindow);
    connect(this,SIGNAL(currentCursorChanged(QCursor*)),subWindow,SLOT(currentCursorChanged(QCursor*)));
    connect(this,SIGNAL(currentToolHasChanged(ToolsBar::SelectableTool)),subWindow,SLOT(currentToolChanged(ToolsBar::SelectableTool)));
    connect(subWindow,SIGNAL(windowStateChanged(Qt::WindowStates,Qt::WindowStates)),subWindow,SLOT(changedStatus(Qt::WindowStates,Qt::WindowStates)));
}
/*void ImprovedWorkspace::activeSubWindowChanged(QMdiSubWindow* subWindow)
{
    SubMdiWindows* tmp = static_cast<SubMdiWindows*>(subWindow);

    tmp->setCurrentTool();

}*/
