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


#include "TextEditAmeliore.h"

static const int MaxHistorySize = 100;

TextEditAmeliore::TextEditAmeliore(QWidget *parent)
    : QTextEdit(parent)
{
}

void TextEditAmeliore::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            QString text = toPlainText().trimmed();
            if (!text.isEmpty())
            {
                m_history.append(text);
                while (m_history.size() > MaxHistorySize)
                    m_history.removeFirst();
                m_histPos = m_history.size();
                emit textValidated(text);
                clear();
            }
        } break;

        case Qt::Key_Up:
            if (e->modifiers() & Qt::ControlModifier)
                emit ctrlUp();
            else if (m_histPos > 0)
            {
                QString text = toPlainText().trimmed();
                if (!text.isEmpty())
                {
                    if (m_histPos == m_history.size())
                        m_history.append(text);
                    else
                        m_history.replace(m_histPos, text);
                }

                m_histPos -= 1;
                setPlainText(m_history[m_histPos]);
            }
            break;

        case Qt::Key_Down:
            if (e->modifiers() & Qt::ControlModifier)
                emit ctrlDown();
            else
            {
                QString text = toPlainText().trimmed();
                if (!text.isEmpty())
                {
                    if (m_histPos == m_history.size())
                        m_history.append(text);
                    else
                        m_history.replace(m_histPos, text);
                }

                if (m_histPos < m_history.size())
                    m_histPos += 1;
                if (m_histPos < m_history.size())
                    setPlainText(m_history[m_histPos]);
                else
                    clear();
            }
            break;

        default:
            QTextEdit::keyPressEvent(e);
    }
}
