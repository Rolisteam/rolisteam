/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include <QPainter>
#include <QVBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QWheelEvent>

#include "vcolortablechooser.h"
#define MAX_COLOR 359
#define MAX_SATURATION 255
////////////////////////////////////////
//Implementation of ValueChooser
////////////////////////////////////////
SaturationChooser::SaturationChooser()
    : m_polygon(QPolygon(3))
{
    m_currentValue=MAX_SATURATION;
    setStyleSheet("background-color: rgb(0,0,0)");
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setMinimumSize(40,10);
    setMaximumSize(maximumWidth(),10);
    m_gradianRect=rect();
    
    
    m_polygon.setPoint(0,0,5);
    m_polygon.setPoint(1,5,9);
    m_polygon.setPoint(2,-5,9);

}
int SaturationChooser::getValue() const
{

    return m_currentValue;

}
void SaturationChooser::mousePressEvent(QMouseEvent* e)
{

    if((e->pos().x()<=width())&&(e->pos().x()>=0))
    {
        m_currentValue = MAX_SATURATION*e->pos().x()/width();
        emit valueChanged(m_currentValue);
        update();
    }

}
void SaturationChooser::mouseMoveEvent(QMouseEvent* e)
{
    if((e->pos().x()<=width())&&(e->pos().x()>=0))
    {
        m_currentValue = MAX_SATURATION*e->pos().x()/width();
        emit valueChanged(m_currentValue);
        update();
    }

}

void SaturationChooser::wheelEvent ( QWheelEvent * event )
{
    int step = event->delta() / 8;
    
    if(step+m_currentValue>MAX_SATURATION)
        m_currentValue=MAX_SATURATION;
    else if(step+m_currentValue<0)
        m_currentValue=0;
    else
        m_currentValue+=step;
    emit valueChanged(m_currentValue);
    update();

}

void SaturationChooser::setColor(QColor& color)
{
    m_color = color;

}

void SaturationChooser::paintEvent ( QPaintEvent * event )
{

    QPainter painter(this);
    QLinearGradient linearGrad(QPointF(0, event->rect().height()/2), QPointF(event->rect().width(), event->rect().height()/2));
    linearGrad.setColorAt(0, Qt::black);
    linearGrad.setColorAt(1, m_color);
    
    QBrush brush(linearGrad);
    m_gradianRect=event->rect();
    m_gradianRect.setHeight(m_gradianRect.height()/2);
    painter.fillRect(m_gradianRect,brush);
    painter.setPen(Qt::black);
    
    int pos=m_currentValue*width()/MAX_SATURATION;
    
    painter.drawPolygon( m_polygon.translated(pos,0));


}
void SaturationChooser::colorHasChanged(int h,int s)
{
    m_color.setHsv(h,s,MAX_SATURATION);
    update();
}


////////////////////////////////////////
//Implementation of ColorTable
////////////////////////////////////////
ColorTable::ColorTable()
{

    setStyleSheet("background-color: rgb(0,0,0)");
    //setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    setMinimumSize(40,40);
    setMaximumSize(100,100);
}
int ColorTable::heightForWidth(int width) const
{
    return width;
}
QSize ColorTable::sizeHint() const
{
    //return QSize(size().width(),heightForWidth(size().width()));
    return QWidget::sizeHint();
}
void ColorTable::paintEvent ( QPaintEvent * event )
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    
    int colorv;
    int saturation;
    QColor color;


   // int min = qMin(,);
    
    for(int i=0;i<width();i++)
    {
        for(int j=0;j<height();j++)
        {
            colorv=MAX_COLOR*j/height();
            saturation=MAX_SATURATION*i/width();
            color.setHsv(colorv,saturation,MAX_SATURATION);
            painter.setPen(color);
            painter.drawPoint(i,j);
        }
    }
    
}
void ColorTable::mousePressEvent(QMouseEvent* e)
{
    if(rect().contains(e->pos()))
    {
        int color=MAX_COLOR*e->pos().y()/height();
        int saturation = MAX_SATURATION*e->pos().x()/width();
        emit dataChanged(color,saturation);
    }
}

void ColorTable::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
}


////////////////////////////////////////
//Implementation of ColorTableChooser
////////////////////////////////////////
ColorTableChooser::ColorTableChooser(QWidget* parent)
    : QWidget(parent)
{
    m_layout=new QVBoxLayout();
    m_layout->setMargin(0);
    m_colorTable=new ColorTable();
    m_valueChooser=new SaturationChooser();
    m_layout->addWidget(m_colorTable);
    m_layout->addWidget(m_valueChooser);
    setLayout(m_layout);
    setStyleSheet("background-color: rgb(0,0,0)");
    
    connect(m_colorTable,SIGNAL(dataChanged(int,int)),this,SLOT(colorHasChanged(int,int)));
    connect(m_colorTable,SIGNAL(dataChanged(int,int)),m_valueChooser,SLOT(colorHasChanged(int,int)));
    connect(m_valueChooser,SIGNAL(valueChanged(int)),this,SLOT(valueHasChanged(int)));
    setMinimumSize(40,45);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    
    m_h=0;
    m_s=MAX_SATURATION;
    
}
void ColorTableChooser::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
}
void ColorTableChooser::valueHasChanged(int v)
{
    m_color.setHsv(m_h,m_s,v);
    emit currentColorChanged(m_color);
}

void ColorTableChooser::colorHasChanged(int h,int s)
{
    m_h=h;
    m_s=s;
    m_color.setHsv(h,s,m_valueChooser->getValue());
    emit currentColorChanged(m_color);
}
