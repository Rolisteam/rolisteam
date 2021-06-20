/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef IMAGEPATHEDITOR_H
#define IMAGEPATHEDITOR_H

#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

namespace rwidgets
{
class ImagePathEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY fileNameChanged)
public:
    explicit ImagePathEditor(const QString& root, QWidget* parent= nullptr);
    ~ImagePathEditor();

    QString filename() const;

    void setPixmap(QPixmap);

    QPixmap getData() const;

signals:
    void editingFinished();
    void fileNameChanged();

public slots:
    void setFilename(const QString& file);

    void openFile();
    void readPixmap(const QString& str);
    void clearPixmap();

protected:
    void setUi();
    void focusInEvent(QFocusEvent* event);

private:
    QLineEdit* m_photoEdit= nullptr;
    QPushButton* m_photoBrowser= nullptr;
    QPushButton* m_cleanButton= nullptr;
    QPixmap m_pixmap;
    QString m_root;
    QString m_filename;
};
} // namespace rwidgets
#endif // IMAGEPATHEDITOR_H
