/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#include "charactersheet/controllers/sheetcontroller.h"

#include "charactersheet/abstractapplicationcontroller.h"

SheetController::SheetController(QObject* parent) : QObject{parent}
{
    auto updateZoomLevel= [this]()
    {
        if(m_adaptWidthToPage)
            setZoomLevel(parentWidth() / imageBgWidth());
    };
    connect(this, &SheetController::parentWidthChanged, this, updateZoomLevel);
    connect(this, &SheetController::imageBgWidthChanged, this, updateZoomLevel);
}

int SheetController::pageMax() const
{
    return m_pageMax;
}

void SheetController::setPageMax(int newPageMax)
{
    if(m_pageMax == newPageMax)
        return;
    m_pageMax= newPageMax;
    emit pageMaxChanged();
}

int SheetController::currentPage() const
{
    return m_currentPage;
}

void SheetController::setCurrentPage(int newCurrentPage)
{
    if(m_currentPage == newCurrentPage || newCurrentPage < 0 || newCurrentPage > pageMax())
        return;
    m_currentPage= newCurrentPage;
    emit currentPageChanged();
}

qreal SheetController::zoomLevel() const
{
    return m_appCtrl ? m_appCtrl->zoomLevel() : 1.0;
}

void SheetController::setZoomLevel(qreal newZoomLevel)
{
    if(m_appCtrl)
        m_appCtrl->setZoomLevel(newZoomLevel);
}

void SheetController::msgToGM(const QString& msg)
{
    if(m_appCtrl)
        m_appCtrl->msgToGM(msg, m_characterId);
}

void SheetController::msgToAll(const QString& msg)
{
    if(m_appCtrl)
        m_appCtrl->msgToAll(msg, m_characterId);
}

void SheetController::rollDice(const QString& cmd, bool gmOnly)
{
    if(m_appCtrl)
        m_appCtrl->rollDice(cmd, m_characterId, gmOnly);
}

void SheetController::nextPage()
{
    setCurrentPage(currentPage() + 1);
}

void SheetController::previousPage()
{
    setCurrentPage(currentPage() - 1);
}

AbstractApplicationController* SheetController::appCtrl() const
{
    return m_appCtrl;
}

void SheetController::setAppCtrl(AbstractApplicationController* newAppCtrl)
{
    if(m_appCtrl == newAppCtrl)
        return;
    m_appCtrl= newAppCtrl;
    emit appCtrlChanged();

    if(m_appCtrl)
        connect(m_appCtrl, &AbstractApplicationController::zoomLevelChanged, this, &SheetController::zoomLevelChanged);
}

qreal SheetController::parentWidth() const
{
    return m_parentWidth;
}

void SheetController::setParentWidth(qreal newParentWidth)
{
    if(qFuzzyCompare(m_parentWidth, newParentWidth))
        return;
    m_parentWidth= newParentWidth;
    emit parentWidthChanged();
}

qreal SheetController::imageBgWidth() const
{
    return m_imageBgWidth;
}

void SheetController::setImageBgWidth(qreal newImageBgWidth)
{
    if(qFuzzyCompare(m_imageBgWidth, newImageBgWidth))
        return;
    m_imageBgWidth= newImageBgWidth;
    emit imageBgWidthChanged();
}

bool SheetController::adaptWidthToPage() const
{
    return m_adaptWidthToPage;
}

void SheetController::setAdaptWidthToPage(bool newAdaptWidthToPage)
{
    if(m_adaptWidthToPage == newAdaptWidthToPage)
        return;
    m_adaptWidthToPage= newAdaptWidthToPage;
    emit adaptWidthToPageChanged();
}

CharacterSheet* SheetController::characterSheetCtrl() const
{
    return m_sheetCtrl;
}

void SheetController::setCharacterSheetCtrl(CharacterSheet* newSheetCtrl)
{
    if(m_sheetCtrl == newSheetCtrl)
        return;
    m_sheetCtrl= newSheetCtrl;
    emit characterSheetCtrlChanged();
}

QString SheetController::characterId() const
{
    return m_characterId;
}

void SheetController::setCharacterId(const QString& newCharacterId)
{
    if(m_characterId == newCharacterId)
        return;
    m_characterId= newCharacterId;
    emit characterIdChanged();
}
