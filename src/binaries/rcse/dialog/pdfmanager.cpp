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
#include "pdfmanager.h"
#include "ui_pdfmanager.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QPdfDocument>
#include <QPdfView>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QtPdf/QPdfPageNavigation>

PdfManager::PdfManager(QWidget* parent) : QDialog(parent), ui(new Ui::PdfManager), m_document(new QPdfDocument())
{
    ui->setupUi(this);

    connect(ui->m_browseFile, &QPushButton::clicked, this, [this]() {
        auto filename= QFileDialog::getOpenFileName(this, tr("Load background from PDF"), QDir::homePath(),
                                                    tr("PDF files (*.pdf)"));
        if(filename.isEmpty())
            return;

        setPdfPath(filename);
    });

    connect(ui->m_resoGroupBox, &QGroupBox::toggled, this, [this](bool checked) {
        ui->m_widthBox->setEnabled(checked);
        ui->m_heightBox->setEnabled(checked);
    });

    ui->m_pdfView->setDocument(m_document.get());

    ui->m_resoGroupBox->setChecked(false);
    ui->m_allPagesCb->setChecked(true);

    ui->m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);
    ui->m_pdfView->setZoomMode(QPdfView::ZoomMode::FitToWidth);

    connect(ui->m_allPagesCb, &QCheckBox::toggled, this, [this](bool checked) {
        if(checked)
        {
            ui->m_currentPageCb->setChecked(false);
            ui->m_pagesCb->setChecked(false);
        }
    });
    connect(ui->m_currentPageCb, &QCheckBox::toggled, this, [this](bool checked) {
        if(checked)
        {
            ui->m_allPagesCb->setChecked(false);
            ui->m_pagesCb->setChecked(false);
        }
    });
    connect(ui->m_pagesCb, &QCheckBox::toggled, this, [this](bool checked) {
        if(checked)
        {
            ui->m_allPagesCb->setChecked(false);
            ui->m_currentPageCb->setChecked(false);
        }
    });

    connect(ui->m_widthBox, &QSpinBox::valueChanged, this, [this](int value) {
        if(!ui->m_resoGroupBox->isChecked())
            return;

        auto sizeF= m_document->pageSize(currentPage());
        auto r= sizeF.height() / sizeF.width();
        const QSignalBlocker blocker(ui->m_heightBox);
        ui->m_heightBox->setValue(value * r);
    });

    connect(ui->m_heightBox, &QSpinBox::valueChanged, this, [this](int value) {
        if(!ui->m_resoGroupBox->isChecked())
            return;

        auto sizeF= m_document->pageSize(currentPage());
        auto r= sizeF.width() / sizeF.height();
        const QSignalBlocker blocker(ui->m_widthBox);
        ui->m_widthBox->setValue(value * r);
    });

    // connect(ui->m_pdfView->viewport(), &QWidget::size);

    // connect(ui->spinBox, &QSpinBox::valueChanged, this, &PdfManager::resolutionChanged);
    // connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SIGNAL(resolutionChanged()));
    /*connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, [=] { emit apply(); });
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, [=] {
        accept();
        // emit accepted();
    });*/
    updateSizeParameter();
}

PdfManager::~PdfManager()
{
    delete ui;
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

const QString PdfManager::pdfPath() const
{
    return ui->m_pathEdit->text();
}

void PdfManager::setPdfPath(const QString& newPdfPath)
{
    if(pdfPath() == newPdfPath)
        return;

    ui->m_pathEdit->setText(newPdfPath);
    m_document->load(newPdfPath);
    emit pdfPathChanged();
    updateSizeParameter();
}

const QList<QImage> PdfManager::images() const
{
    QList<QImage> m_images;

    bool grayScale= ui->m_grayScale->isChecked();

    QSize areaSize= ui->m_pdfView->viewport()->size();
    QSize widgetSize= ui->m_pdfView->size();

    auto v= ui->m_pdfView->verticalScrollBar()->pageStep();
    auto h= ui->m_pdfView->horizontalScrollBar()->pageStep();

    // Compute Size
    QSize size= QSize(ui->m_widthBox->value(), ui->m_heightBox->value());

    // Compute pages
    QList<int> pages;

    if(ui->m_allPagesCb->isChecked())
    {
        for(auto i= 0; i < m_document->pageCount(); ++i)
            pages.append(i);
    }
    else if(ui->m_currentPageCb->isChecked())
    {
        auto nav= ui->m_pdfView->pageNavigation();
        pages.append(nav->currentPage());
    }
    else if(ui->m_pagesCb->isChecked())
    {
        auto pagesList= ui->m_pageLine->text();
        auto lst= pagesList.split(";");
        QRegularExpression range;
        range.setPattern("(\\d+)-(\\d+)");
        for(const auto& line : lst)
        {
            auto match= range.match(line);
            if(match.hasMatch())
            {
                auto start= match.captured(1).toInt();
                auto end= match.captured(2).toInt();

                if(end < start)
                    continue;

                for(auto i= start; i <= end; ++i)
                {
                    pages.append(i);
                }
            }
            else
            {
                bool ok;
                auto v= line.toInt(&ok);
                if(ok)
                    pages.append(v);
            }
        }
    }

    // Compute images
    QPdfDocumentRenderOptions options;
    if(grayScale)
        options.setRenderFlags(QPdf::RenderGrayscale);

    options.setScaledSize(size);
    for(auto page : pages)
    {
        auto img= m_document->render(page, size, options);

        if(!img.isNull())
            m_images.append(img);
    }

    return m_images;
}

void PdfManager::updateSizeParameter()
{
    if(ui->m_resoGroupBox->isChecked())
        return;

    auto sizeF= m_document->pageSize(currentPage()) * (1. + 1. / 3.);
    ui->m_widthBox->setValue(sizeF.width());
    ui->m_heightBox->setValue(sizeF.height());
}

int PdfManager::currentPage() const
{
    auto nav= ui->m_pdfView->pageNavigation();
    if(!nav)
        return 0;
    return nav->currentPage();
}
