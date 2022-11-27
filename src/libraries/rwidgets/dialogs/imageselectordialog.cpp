/***************************************************************************
 * Copyright (C) 2015 by Renaud Guezennec                                   *
 * https://rolisteam.org/contact                      *
 *                                                                          *
 *  This file is part of rcm                                                *
 *                                                                          *
 * Rolisteam is free software; you can redistribute it and/or modify              *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "imageselectordialog.h"
#include "ui_imageselectordialog.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QPixmap>
#include <QPushButton>
#include <QUrl>

#include "controller/view_controller/imageselectorcontroller.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"
#include "rwidgets/customs/overlay.h"

ImageSelectorDialog::ImageSelectorDialog(ImageSelectorController* ctrl, QWidget* parent, const QString& defaultPath)
    : QDialog(parent), m_ctrl(ctrl), ui(new Ui::ImageSelectorDialog), m_overlay(new Overlay())
{
    ui->setupUi(this);
    if(!defaultPath.isEmpty())
        ui->m_textEdit->setText(defaultPath);

    auto download= [this]() { m_ctrl->downloadImageFrom(QUrl::fromUserInput(ui->m_textEdit->text())); };
    connect(ui->m_textEdit, &QLineEdit::textEdited, m_ctrl, download);
    connect(ui->m_downloadAct, &QAction::triggered, m_ctrl, download);
    connect(ui->m_pasteAct, &QAction::triggered, m_ctrl, &ImageSelectorController::imageFromClipboard);
    connect(ui->m_openFileAct, &QAction::triggered, this, &ImageSelectorDialog::openImage);
    connect(m_ctrl, &ImageSelectorController::titleChanged, this,
            [this]() { ui->m_titleLineEdit->setText(m_ctrl->title()); });
    connect(ui->m_titleLineEdit, &QLineEdit::textEdited, this,
            [this]() { m_ctrl->setTitle(ui->m_titleLineEdit->text()); });

    auto func= [this](const QRect& rect) {
        // qreal scale= m_ctrl->pixmap().size().width() / m_imageViewerLabel->rect().width();
        // QRect scaledRect(rect.x() * scale, rect.y() * scale, rect.width() * scale, rect.height() * scale);
        m_ctrl->setRect(rect);
    };
    connect(m_overlay.get(), &Overlay::selectedRectChanged, m_ctrl, func);


    setAcceptDrops(m_ctrl->canDrop());

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->m_downloadPush->setDefaultAction(ui->m_downloadAct);
    ui->m_pasteBtn->setDefaultAction(ui->m_pasteAct);
    ui->m_openBtn->setDefaultAction(ui->m_openFileAct);

    ui->m_downloadPush->setVisible(m_ctrl->canDownload());
    ui->m_pasteBtn->setVisible(m_ctrl->canPaste());

    connect(m_ctrl, &ImageSelectorController::contentToPasteChanged, ui->m_pasteAct,
            [this]() { ui->m_pasteAct->setEnabled(m_ctrl->hasContentToPaste()); });

    ui->m_pasteAct->setEnabled(m_ctrl->hasContentToPaste());

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->scrollArea->setAlignment(Qt::AlignCenter);
    m_imageViewerLabel= new QLabel(this);
    auto pix= QPixmap(QString::fromUtf8(":/resources/images/preview.png"));
    m_imageViewerLabel->setPixmap(pix);
    m_imageViewerLabel->setLineWidth(0);
    m_imageViewerLabel->setFrameStyle(QFrame::NoFrame);
    m_imageViewerLabel->setScaledContents(true);
    m_imageViewerLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageViewerLabel->resize(pix.size());
    m_overlay->setParent(m_imageViewerLabel);

    m_overlay->resize(m_imageViewerLabel->rect().size());

    // m_overlay->setSelectedRect(m_imageViewerLabel->rect());

    m_overlay->setRatio(m_ctrl->shape() == ImageSelectorController::Square ? Overlay::Ratio::Ratio_Square :
                                                                             Overlay::Ratio::Ratio_Unconstrained);
    m_overlay->initRect();

    connect(m_ctrl, &ImageSelectorController::imageDataChanged, this, [this]() {
        if(m_ctrl->isMovie())
        {
            auto movie= m_ctrl->movie();
            m_imageViewerLabel->setMovie(movie);
            m_imageViewerLabel->resize(movie->frameRect().size());
        }
        else
        {
            auto pix= m_ctrl->pixmap();
            m_imageViewerLabel->setPixmap(pix);
            m_imageViewerLabel->resize(pix.size());
        }
        auto button= ui->buttonBox->button(QDialogButtonBox::Ok);

        if(button)
            button->setEnabled(m_ctrl->validData() && m_ctrl->rectInShape());

        m_overlay->setVisible(!m_ctrl->dataInShape());
        resizeLabel();
        update();
    });

    ui->scrollArea->setWidget(m_imageViewerLabel);

    if(m_ctrl->askPath())
    {
        openImage();
    }
    if(m_overlay)
        m_ctrl->setRect(m_overlay->selectedRect());
}

ImageSelectorDialog::~ImageSelectorDialog()
{
    delete ui;
}

void ImageSelectorDialog::openImage()
{
    auto filename
        = QFileDialog::getOpenFileName(this, tr("Open Image file"), m_ctrl->currentDir(),
                                       QString("Image File (%1)").arg(helper::utils::allSupportedImageFormatFilter()));

    if(!filename.isEmpty())
    {
        m_ctrl->setAddress(filename);
        ui->m_textEdit->setText(filename);
        m_ctrl->openImageFromFile();
    }
}

void ImageSelectorDialog::resizeLabel()
{
    if(!isVisible())
        return;

    auto pix= m_ctrl->pixmap();
    auto const sImg= pix.size();

    int w= ui->scrollArea->viewport()->rect().width();
    int h= ui->scrollArea->viewport()->rect().height();

    if(sImg.width() < w)
    {
        w= sImg.width();
    }
    if(sImg.height() < h)
    {
        h= sImg.height();
    }

    double const ratioImage= static_cast<double>(pix.size().width()) / pix.size().height();
    double const ratioImageBis= static_cast<double>(pix.size().height()) / pix.size().width();

    if(w > h * ratioImage)
    {
        m_imageViewerLabel->resize(h * ratioImage, h);
    }
    else
    {
        m_imageViewerLabel->resize(w, w * ratioImageBis);
    }

    m_overlay->resize(m_imageViewerLabel->rect().size());
    m_overlay->setSelectedRect(m_overlay->rect());
}

void ImageSelectorDialog::resizeEvent(QResizeEvent* event)
{
    resizeLabel();
    QDialog::resizeEvent(event);
}

void ImageSelectorDialog::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasUrls() || event->mimeData()->hasImage())
    {
        event->acceptProposedAction();
    }
    QDialog::dragEnterEvent(event);
}

void ImageSelectorDialog::dropEvent(QDropEvent* event)
{
    const QMimeData* data= event->mimeData();
    if(!data->hasUrls() && !data->hasImage())
        return;

    QList<QUrl> list= data->urls();
    auto img= qvariant_cast<QImage>(data->imageData());
    if(!img.isNull())
    {
        m_ctrl->setAddress({});
        m_ctrl->setImageData(IOHelper::imageToData(img));
    }
    else if(!list.isEmpty())
    {
        auto first= list.first();
        m_ctrl->downloadImageFrom(first);
    }
    event->acceptProposedAction();
}

void ImageSelectorDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    resizeLabel();
}
