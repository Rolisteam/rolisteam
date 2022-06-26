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
#ifndef BUSYINDICATORDIALOG_H
#define BUSYINDICATORDIALOG_H

#include <QDialog>
#include <QWidget>
#include <common_widgets/common_widgets_global.h>

class COMMON_WIDGET_EXPORT BusyIndicatorDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    BusyIndicatorDialog(const QString& title, const QString& text, const QString& pathMovie, QWidget* parent= nullptr);

    const QString& title() const;
    void setTitle(const QString& newTitle);

    const QString& text() const;
    void setText(const QString& newText);

signals:
    void titleChanged();
    void textChanged();

private:
    void setUpUi();

private:
    QString m_title;
    QString m_text;
    QString m_path;
};

#endif // BUSYINDICATORDIALOG_H
