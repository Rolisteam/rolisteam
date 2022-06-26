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
#include "common_widgets/busyindicatordialog.h"

#include <QLabel>
#include <QMovie>
#include <QProgressBar>
#include <QVBoxLayout>

BusyIndicatorDialog::BusyIndicatorDialog(const QString& title, const QString& text, const QString& pathMovie,
                                         QWidget* parent)
    : QDialog(parent), m_title(title), m_text(text), m_path(pathMovie)
{
    setModal(true);
    setUpUi();
}
void BusyIndicatorDialog::setUpUi()
{
    setTitle(m_title);
    setLayout(nullptr);
    auto l= new QVBoxLayout(this);
    auto lbl= new QLabel(this);
    lbl->setText(m_text);
    l->addWidget(lbl);

    QMovie* movie= new QMovie(m_path, {}, this);

    auto lbl2= new QLabel(this);
    lbl2->setMaximumHeight(200);
    lbl2->setMaximumWidth(200);
    lbl2->setScaledContents(true);
    lbl2->setMovie(movie);
    l->addWidget(lbl2);
    setLayout(l);

    movie->start();
}

const QString& BusyIndicatorDialog::text() const
{
    return m_text;
}

void BusyIndicatorDialog::setText(const QString& newText)
{
    if(m_text == newText)
        return;
    m_text= newText;
    emit textChanged();
}

const QString& BusyIndicatorDialog::title() const
{
    return m_title;
}

void BusyIndicatorDialog::setTitle(const QString& newTitle)
{
    if(m_title == newTitle)
        return;
    m_title= newTitle;
    emit titleChanged();
}
