/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
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


#ifndef TEXTEDIT_AMELIORE_H
#define TEXTEDIT_AMELIORE_H

#include <QTextEdit>
#include <QKeyEvent>

/**
 * @brief The TextEditAmeliore class
 */
class TextEditAmeliore : public QTextEdit
{
    Q_OBJECT

    public :
        TextEditAmeliore(QWidget *parent = 0);

    signals :
        void textValidated(QString textHtml,QString text);
        void ctrlUp();
        void ctrlDown();
    void receivedFocus();

    protected :
        void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

    private:
        QStringList m_history;
        int m_histPos;
};

#endif
