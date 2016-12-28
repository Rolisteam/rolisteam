/***************************************************************************
    *   Copyright (C) 2016 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                                      *
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
#include <QPushButton>

#include "pdfmanager.h"
#include "ui_pdfmanager.h"

PdfManager::PdfManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdfManager)
{
    ui->setupUi(this);
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SIGNAL(resolutionChanged()));
    //connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SIGNAL(resolutionChanged()));
    connect(ui->buttonBox->button(QDialogButtonBox::Apply),&QPushButton::clicked,[=]{
        emit apply();
    });
   /* connect(ui->buttonBox->button(QDialogButtonBox::Ok),&QPushButton::clicked,[=]{
        accept();
        //emit accepted();
    });*/
}

PdfManager::~PdfManager()
{
    delete ui;
}

qreal PdfManager::getDpi()
{
    return (qreal)ui->spinBox->value();
}

bool PdfManager::hasResolution()
{
    return ui->m_resolutionCheck->isChecked();
}

int PdfManager::getWidth()
{
    return ui->m_widthBox->value();
}

int PdfManager::getHeight()
{
    return ui->m_heightBox->value();
}

void PdfManager::setHeight(int h)
{
    ui->m_heightBox->setValue(h);
}

void PdfManager::setWidth(int w)
{
    ui->m_widthBox->setValue(w);
}
