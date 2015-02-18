/***************************************************************************
 *	Copyright (C) 2011 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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
#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QtDesigner/QDesignerExportWidget>
/**
  * @brief composite widget which brings together a lineEdit and a pushbutton to select path (directory or file).
  */
class QDESIGNER_WIDGET_EXPORT PathChooser : public QWidget
{
    Q_OBJECT
public:
        /**
          * @brief default constructor
          */
    explicit PathChooser(QWidget *parent = 0);
    /**
      * @brief path accessor
      */
    QString getPath();
    /**
      * @brief defines the title of the fileselector
      */
    void setTitle(QString& t);
    /**
      * @brief defines filters (only use when file is wanted)
      */
    void setFilters(QString& t);

signals:
    /**
      * @brief emit when the path has changed
      */
    void pathChanged();

public slots:
    /**
      * @brief set the current path
      */
    void setPath(QString m);
private slots:
    /**
      * @brief manages internally the button click.
      */
    void onButton();
    /**
      * @brief called after path editing
      */
    void editPath();
private:
    /**
      * @brief called for initialising the User Interface
      */
    void setupUi();
    /**
      * @brief stores the path
      */
    QString m_path;
    /**
      * @brief lineedit widget
      */
    QLineEdit* m_line;
    /**
      * @brief pushbutton widget
      */
    QPushButton* m_button;
    /**
      * @brief horizontal layout
      */
    QHBoxLayout* m_layout;
    /**
      * @brief directory or file wanted
      */
    bool m_isDirectory;
    /**
      * @brief stores file filters
      */
    QString m_filters;
    /**
      * @brief stores title.
      */
    QString m_title;
};

#endif // PATHCHOOSER_H
