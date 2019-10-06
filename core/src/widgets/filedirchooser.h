/*************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *                                                                       *
 *    http://www.rolisteam.org/                                          *
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

#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include <QWidget>

/**
 * @brief A LineEdit with a button to choose a file or directory. It displays a QLineEdit and a QPushButton allowing to
 * show the current selection and to browse to select something else.
 *
 */
class FileDirChooser : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief FileDirChooser
     * @param isDirectory
     * @param parent
     */
    FileDirChooser(bool isDirectory= true, QWidget* parent= nullptr);
    ~FileDirChooser();
    /**
     * @brief setPath define the value of the widget. Current selection.
     * @param dirname
     */
    void setPath(const QString& dirname);
    /**
     * @brief path
     * @return the path selected by the user.
     */
    QString path() const;
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

signals:
    /**
     * @brief pathChanged
     */
    void pathChanged();

private slots:
    /**
     * @brief browse
     */
    void browse();

private:
    QLineEdit* m_lineEdit;
    QString m_filter;
    bool m_directory;
};

#endif // DIRCHOOSER_H
