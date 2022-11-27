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
#ifndef IMAGESELECTORDIALOG_H
#define IMAGESELECTORDIALOG_H

#include "rwidgets_global.h"
#include <QDialog>
#include <QLabel>
#include <QPointer>
#include <memory>
class Overlay;
namespace Ui
{
class ImageSelectorDialog;
}
class ImageSelectorController;
class QDragEnterEvent;
class QDropEvent;
class RWIDGET_EXPORT ImageSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageSelectorDialog(ImageSelectorController* ctrl,  QWidget* parent= nullptr, const QString &defaultPath={});
    virtual ~ImageSelectorDialog();

private slots:
    void openImage();

protected:
    void resizeLabel();
    void resizeEvent(QResizeEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    QPointer<ImageSelectorController> m_ctrl;
    Ui::ImageSelectorDialog* ui;
    std::unique_ptr<Overlay> m_overlay;

    qreal m_ratio= 1.;
    qreal m_zoomLevel= 1.0;
    QLabel* m_imageViewerLabel;
    QString m_postingStr;
};

#endif // IMAGESELECTORDIALOG_H
