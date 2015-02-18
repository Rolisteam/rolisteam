/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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

//#include <QTextStream>

#define GRAY_SCALE 191
/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
WorkspaceAmeliore::WorkspaceAmeliore(QWidget *parent)
: QMdiArea(parent)
{
    m_preferences =  PreferencesManager::getInstance();
    QString fichierImage = QDir::homePath() + "/." + m_preferences->value("Application_Name","rolisteam").toString() + "/" + m_preferences->value("Application_Name","rolisteam").toString() + ".bmp";

    if (!QFile::exists(fichierImage))
    {
        fichierImage = ":/resources/icones/fond workspace macos.bmp";
    }

    m_color.setRgb(GRAY_SCALE,GRAY_SCALE,GRAY_SCALE);
    m_background.setColor(m_color);
    setBackground(m_background);


    m_variableSizeBackground = new QPixmap(size());


    m_variableSizeBackground->fill(m_color);
    QPainter painter(m_variableSizeBackground);


    m_backgroundPicture = new QPixmap(fichierImage);

    painter.drawPixmap(0,0,m_backgroundPicture->width(),m_backgroundPicture->height(),*m_backgroundPicture);
    setBackground(QBrush(*m_variableSizeBackground));
}

WorkspaceAmeliore::~WorkspaceAmeliore()
{
    delete m_backgroundPicture;
    delete m_variableSizeBackground;
}

void WorkspaceAmeliore::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
    if((m_variableSizeBackground)&&(m_variableSizeBackground->size()==this->size()))
    {
        return;
    }
    if(m_variableSizeBackground)
    {
        delete m_variableSizeBackground;
    }

    m_variableSizeBackground = new QPixmap(size());
    m_variableSizeBackground->fill(m_color);
    QPainter painter(m_variableSizeBackground);

    painter.drawPixmap(0,0,m_backgroundPicture->width(),m_backgroundPicture->height(),*m_backgroundPicture);
    setBackground(QBrush(*m_variableSizeBackground));

    QMdiArea::resizeEvent(event);
}
QWidget*  WorkspaceAmeliore::addWindow(QWidget* child)
{
    QMdiSubWindow* sub = addSubWindow(child);
    sub->setAttribute(Qt::WA_DeleteOnClose, false);
    child->setAttribute(Qt::WA_DeleteOnClose, false);
    return sub;
}
QWidget* WorkspaceAmeliore::activeWindow()
{
    return currentSubWindow();
}
