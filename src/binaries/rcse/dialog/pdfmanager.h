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

#ifndef PDFMANAGER_H
#define PDFMANAGER_H

#include <QDialog>
#include <QImage>
#include <QList>
#include <memory>

namespace Ui
{
class PdfManager;
}
class QPdfDocument;
class PdfManager : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QList<QImage> images READ images NOTIFY imageChanged)
    Q_PROPERTY(QString pdfPath READ pdfPath WRITE setPdfPath NOTIFY pdfPathChanged)
public:
    explicit PdfManager(QWidget* parent= 0);
    virtual ~PdfManager();

    int getWidth();
    int getHeight();

    void setHeight(int h);
    void setWidth(int w);

    const QString pdfPath() const;
    void setPdfPath(const QString& newPdfPath);

    const QList<QImage> images() const;

protected:
    void updateSizeParameter();

    int currentPage() const;

signals:
    void resolutionChanged();
    void apply();
    void pdfPathChanged();
    void imageChanged();

private:
    Ui::PdfManager* ui;
    std::unique_ptr<QPdfDocument> m_document;
};

#endif // PDFMANAGER_H
