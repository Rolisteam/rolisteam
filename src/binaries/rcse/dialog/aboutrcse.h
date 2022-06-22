/***************************************************************************
* Copyright (C) 2018 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#ifndef ABOUTRCSE_H
#define ABOUTRCSE_H

#include <QDialog>

namespace Ui {
class AboutRcse;
}

class AboutRcse : public QDialog
{
    Q_OBJECT

public:
    explicit AboutRcse(QString verison, QWidget *parent);
    ~AboutRcse();

    QString version() const;
    void setVersion(const QString &version);

private:
    Ui::AboutRcse *ui;
    QString m_version;
};

#endif // ABOUTRCSE_H
