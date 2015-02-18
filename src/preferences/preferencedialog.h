/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>
class PreferencesManager;
class QAbstractButton;

namespace Ui {
    class PreferenceDialog;
}
/**
  * @brief Display all required item to manage preferences
  *
  */
class PreferenceDialog : public QDialog {
    Q_OBJECT
public:
    /**
      * @brief default constructor.
      * @return parent QWidget
      */
    PreferenceDialog(QWidget *parent = 0);
    /**
      * @brief destructor.
      */
    ~PreferenceDialog();

signals:
    /**
      * @brief emited when preferences have been changed
      */
    void preferencesChanged();

protected slots:
    /**
      * @brief is called when user wants to change the backgound color.
      */
    void changeBackgroundImage();
    /**
      * @brief Save all data into QSettings.
      */
    void applyAllChanges(QAbstractButton * button);
protected:
    /**
      * @brief callback receiving the change event.
      * @return e QEvent describing the context of the event
      */
    void changeEvent(QEvent *e);

private:
    /**
      * Pointer to the GUI made with QDesigner
      */
    Ui::PreferenceDialog *ui;

    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
};

#endif // PREFERENCEDIALOG_H
