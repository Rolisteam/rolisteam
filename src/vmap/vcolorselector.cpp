/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                                 *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include <QtWidgets>
#include <QButtonGroup>

#include "vcolorselector.h"


#include "preferences/preferencesmanager.h"

VColorLabel::VColorLabel(QWidget * parent)
    : QAbstractButton(parent)
{
    setMinimumSize(40,40);
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}
void VColorLabel::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    emit clickedColor(this->palette().color(QPalette::Window));
    QAbstractButton::mousePressEvent(ev);
}
void VColorLabel::resizeEvent(QResizeEvent * event)
{
    /*int length = width()>height() ? height() : width();
    setFixedSize(length,length);*/
    QWidget::resizeEvent(event);
}
void VColorLabel::mouseDoubleClickEvent (QMouseEvent *event)
{
    Q_UNUSED(event);
    emit doubledclicked();
    QAbstractButton::mouseDoubleClickEvent(event);
}
void VColorLabel::paintEvent( QPaintEvent * event )
{
    /* QPainter painter(this);
    painter.fillRect(rect(),this->palette().color(QPalette::Window));*/
    QWidget::paintEvent(event);
    
}
// end of VColorLabel


BackgroundButton::BackgroundButton(QPixmap* p,QWidget * parent )
    : QPushButton(parent),m_background(p)
{
    /* setIcon(QIcon(*m_background));
    setIconSize(QSize(rect().width()-2,rect().height()-2));*/
    setCheckable(true);
}

void BackgroundButton::paintEvent( QPaintEvent * event )
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QRect r=rect();
    r.setBottom(r.bottom());
    r.setTop(r.top());
    r.setLeft(r.left());
    r.setRight(r.right());
    painter.drawImage(rect(),m_background->toImage(),m_background->rect());
    if(isChecked())
        painter.drawRect(rect().adjusted(0,0,-1,-1));
    //QPushButton::paintEvent(event);
}


VColorSelector::VColorSelector(QWidget *parent)
    : QWidget(parent)
{
    m_options = PreferencesManager::getInstance();
    
    
    
    // Creation du layout principale
    QVBoxLayout *selecteurLayout = new QVBoxLayout(this);
    
    m_currentColorLabel = new VColorLabel(this);
    m_currentColorLabel->setMaximumSize(200,200);
    m_currentColorLabel->setPalette(QPalette(QColor(0,0,0)));
    m_currentColorLabel->setToolTip(tr("Predefine Color 1"));
    m_currentColorLabel->setAutoFillBackground(true);
    connect(m_currentColorLabel,SIGNAL(doubledclicked()),this,SLOT(VColorSelectorDialog()));
    
    m_currentColor = QColor(0,0,0);
    
    
    
    m_colorTableChooser = new ColorTableChooser(this);
    //m_colorTableChooser->setSizePolicy(QSizePolicy::Fixed);
    m_colorTableChooser->setMaximumSize(200,200);
    
    
    selecteurLayout->addWidget(m_currentColorLabel,1);
    //selecteurLayout->setAlignment(m_currentColorLabel, Qt::AlignHCenter | Qt::AlignTop);
    selecteurLayout->addWidget(m_colorTableChooser,1);
    connect(m_colorTableChooser,SIGNAL(currentColorChanged(QColor)),this,SLOT(selectColor(QColor)));
    
    setLayout(selecteurLayout);
}
void VColorSelector::selectColor(const QColor& color)
{
    
    m_currentColorLabel->setPalette(QPalette(color));
    m_currentColor = color;
    
    emit currentColorChanged(m_currentColor);
    emit currentModeChanged(NORMAL);
}
void VColorSelector::setCurrentColor(QColor& color)
{
    
    //m_currentColorLabel->clear();
    m_currentColorLabel->setPalette(QPalette(color));
    
    m_currentColorLabel->setToolTip(tr("Red: %1, Green: %2, Blue: %3").arg(color.red()).arg(color.green()).arg(color.blue()));

    m_currentColor = color;
    emit currentColorChanged(m_currentColor);
}
QColor& VColorSelector::currentColor()
{
    return m_currentColor;
    
}







void VColorSelector::VColorSelectorDialog()
{
    QColor color = QColorDialog::getColor(m_currentColor);
    if (color.isValid())
    {
        //m_currentColorLabel->setPalette(QPalette(color));
        setCurrentColor(color);
        //m_currentColorLabel->setToolTip(tr("Red: %1, Green: %2, Blue: %3").arg(color.red()).arg(color.green()).arg(color.blue()));
    }
}





