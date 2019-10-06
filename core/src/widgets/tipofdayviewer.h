/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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
#ifndef TIPOFDAYVIEWER_H
#define TIPOFDAYVIEWER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QWidget>

#include "ui_tipofdayviewer.h"

namespace Ui
{
    class TipOfDayViewer;
}

class TipOfDayViewer : public QDialog
{
    Q_OBJECT

public:
    TipOfDayViewer(QString title, QString msg, QString url, QWidget* parent= nullptr);

    bool dontshowAgain() const;
    void setDontshowAgain(bool dontshowAgain);

private:
    Ui::TipOfDayViewer* m_ui;
    bool m_dontshowAgain;
};

#endif // TIPOFDAYVIEWER_H
