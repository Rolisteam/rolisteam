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
#ifndef SHEETCONTROLLER_H
#define SHEETCONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QQmlEngine>

#include <charactersheet/charactersheet.h>
#include <charactersheet/charactersheet_global.h>

class AbstractApplicationController;
class CHARACTERSHEET_EXPORT SheetController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int pageMax READ pageMax WRITE setPageMax NOTIFY pageMaxChanged REQUIRED)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(qreal parentWidth READ parentWidth WRITE setParentWidth NOTIFY parentWidthChanged)
    Q_PROPERTY(qreal imageBgWidth READ imageBgWidth WRITE setImageBgWidth NOTIFY imageBgWidthChanged)
    Q_PROPERTY(bool adaptWidthToPage READ adaptWidthToPage WRITE setAdaptWidthToPage NOTIFY adaptWidthToPageChanged)
    Q_PROPERTY(AbstractApplicationController* appCtrl READ appCtrl WRITE setAppCtrl NOTIFY appCtrlChanged REQUIRED)
    Q_PROPERTY(CharacterSheet* characterSheetCtrl READ characterSheetCtrl WRITE setCharacterSheetCtrl NOTIFY
                   characterSheetCtrlChanged REQUIRED FINAL)
    Q_PROPERTY(QString characterId READ characterId WRITE setCharacterId NOTIFY characterIdChanged FINAL)
    QML_ELEMENT
public:
    explicit SheetController(QObject* parent= nullptr);

    int pageMax() const;
    void setPageMax(int newPageMax);

    int currentPage() const;
    void setCurrentPage(int newCurrentPage);

    qreal zoomLevel() const;
    void setZoomLevel(qreal newZoomLevel);

    Q_INVOKABLE void msgToGM(const QString& msg);
    Q_INVOKABLE void msgToAll(const QString& msg);
    Q_INVOKABLE void rollDice(const QString& cmd, bool gmOnly= false);
    Q_INVOKABLE void nextPage();
    Q_INVOKABLE void previousPage();

    AbstractApplicationController* appCtrl() const;
    void setAppCtrl(AbstractApplicationController* newAppCtrl);

    qreal parentWidth() const;
    void setParentWidth(qreal newParentWidth);

    qreal imageBgWidth() const;
    void setImageBgWidth(qreal newImageBgWidth);

    bool adaptWidthToPage() const;
    void setAdaptWidthToPage(bool newAdaptWidthToPage);

    CharacterSheet* characterSheetCtrl() const;
    void setCharacterSheetCtrl(CharacterSheet* newSheetCtrl);

    QString characterId() const;
    void setCharacterId(const QString& newCharacterId);

signals:
    void pageMaxChanged();
    void currentPageChanged();
    void zoomLevelChanged();
    void appCtrlChanged();
    void parentWidthChanged();
    void imageBgWidthChanged();
    void adaptWidthToPageChanged();
    void characterSheetCtrlChanged();

    void characterIdChanged();

private:
    QPointer<AbstractApplicationController> m_appCtrl;
    int m_pageMax{0};
    int m_currentPage{0};
    qreal m_parentWidth{0};
    qreal m_imageBgWidth{0};
    bool m_adaptWidthToPage{true};
    QPointer<CharacterSheet> m_sheetCtrl;
    QString m_characterId;
};

#endif // SHEETCONTROLLER_H
