/*************************************************************************
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#ifndef CENTEREDCHECKBOX_H
#define CENTEREDCHECKBOX_H

#include <QCheckBox>
#include <QWidget>
/**
 * @brief The CenteredCheckBox class is a simple widget to display checkbox in the middle of their parent.
 * It is useful for ItemDelegate.
 */
class CenteredCheckBox : public QWidget
{
    Q_OBJECT
public:
    CenteredCheckBox(QWidget* parent= nullptr);

    bool isCheckedDelegate() const;
    void setCheckedDelegate(bool);

signals:
    void commitEditor();

private:
    QCheckBox* m_editorCheckBox;
};

#endif // CENTEREDCHECKBOX_H
