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


#include <QRegularExpression>
#include "chat/improvedtextedit.h"
#include <QDebug>

static const int MaxHistorySize = 100;

ImprovedTextEdit::ImprovedTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
}

void ImprovedTextEdit::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
        /// @warning changing the method to get the text
			//QString textHtml = toHtml().trimmed();
            QString text = toPlainText().trimmed();
            if (!text.isEmpty())
            {
				bool hasHtml = false;
                m_history.append(text);
                while (m_history.size() > MaxHistorySize)
                {
                    m_history.removeFirst();
                }
                m_histPos = m_history.size();

                QString result = text;
                QString result2 = text.replace(QRegularExpression("((?:https?)://\\S+)"), "<a href=\"\\1\">\\1</a>");
				Q_UNUSED(result2);
                if(text==result)
                {
                    text.replace(QRegularExpression("((?:www)\\S+)"), "<a href=\"http://\\1\">\\1</a>");
                }
				if(text!=toPlainText().trimmed())
				{
					hasHtml = true;
				}
				emit textValidated(hasHtml,text);
                clear();
            }
        } break;

        case Qt::Key_Up:
            if (e->modifiers() & Qt::ControlModifier)
            {
                emit ctrlUp();
            }
            else if ((m_histPos > 0)&&(!m_history.empty()))
            {
                /// @warning changing the method to get the text
                QString text = toHtml().trimmed();
                if (!text.isEmpty())
                {
                    if (m_histPos == m_history.size())
                        m_history.append(text);
                    else
                        m_history.replace(m_histPos, text);
                }

                m_histPos -= 1;
                //setHtml(m_history[m_histPos]);
                setText(m_history[m_histPos]);
            }
            break;

        case Qt::Key_Down:
            if (e->modifiers() & Qt::ControlModifier)
            {
                emit ctrlDown();
            }
            else if(!m_history.empty())
            {
                /// @warning changing the method to get the text
                QString text = toHtml().trimmed();
                if((!text.isEmpty()))
                {
                    if (m_histPos == m_history.size())
                        m_history.append(text);
                    else
                        m_history.replace(m_histPos, text);
                }

                if (m_histPos < m_history.size())
                    m_histPos += 1;
                if (m_histPos < m_history.size())
                    setText(m_history[m_histPos]);
                else
                    clear();
            }
            break;

        default:
            QTextEdit::keyPressEvent(e);
    }
}
void ImprovedTextEdit::focusInEvent(QFocusEvent *e)
{
    if(e->gotFocus())
    {
        emit receivedFocus();
    }
    QTextEdit::focusInEvent(e);
}
