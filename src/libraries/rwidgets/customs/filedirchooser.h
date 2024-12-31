/*************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *                                                                       *
 *    https://rolisteam.org/                                          *
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
#ifndef DIRCHOOSER_H
#define DIRCHOOSER_H

#include "rwidgets_global.h"
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include <QWidget>
/**
 * @brief A LineEdit with a button to choose a file or directory. It displays a QLineEdit and a QPushButton allowing to
 * show the current selection and to browse to select something else.
 *
 */
class RWIDGET_EXPORT FileDirChooser : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief FileDirChooser
     * @param isDirectory
     * @param parent
     */
    FileDirChooser(QWidget* parent= nullptr, const QString& ext= QString(), bool isDirectory= true);
    ~FileDirChooser();
    /**
     * @brief setPath define the value of the widget. Current selection.
     * @param dirname
     */
    void setUrl(const QUrl& dirname);

    /**
     * @brief setFilter defines filter to select the file.
     * @param filter
     */
    void setFilter(const QString& filter);
    /**
     * @brief getFilter
     * @return
     */
    QString getFilter();
    /**
     * @brief setMode
     * @param isDirectory
     */
    void setMode(bool isDirectory);

    QUrl url() const;

    bool isValid() const;

    void setExt(const QString& ext);

signals:
    /**
     * @brief pathChanged
     */
    void pathChanged(const QUrl& path);

public slots:
    void browse();

private:
    QLineEdit* m_lineEdit= nullptr;
    QString m_filter;
    QString m_ext;
    bool m_directory= true;
};

#endif // DIRCHOOSER_H
