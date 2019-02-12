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
#include <QHBoxLayout>

#include "centeredcheckbox.h"
CenteredCheckBox::CenteredCheckBox(QWidget* wid) : QWidget(wid)
{
    QHBoxLayout* layout= new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    layout->setAlignment(Qt::AlignCenter);

    m_editorCheckBox= new QCheckBox();
    layout->addWidget(m_editorCheckBox);

    connect(m_editorCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(commitEditor()));
}

bool CenteredCheckBox::isCheckedDelegate() const
{
    bool checked= false;
    switch(m_editorCheckBox->checkState())
    {
    case Qt::Unchecked:
        checked= false;
        break;
    case Qt::PartiallyChecked:
    case Qt::Checked:
        checked= true;
        break;
    }
    return checked;
}

void CenteredCheckBox::setCheckedDelegate(bool checked)
{
    if(checked)
    {
        m_editorCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_editorCheckBox->setCheckState(Qt::Unchecked);
    }
    update();
}
