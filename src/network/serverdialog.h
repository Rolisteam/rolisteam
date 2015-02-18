/***************************************************************************
    *   Copyright (C) 2011 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>

namespace Ui {
class ServerDialog;
}
class PreferencesManager;
/**
    * @brief small dialog to ask some parameters to the user.
    */
class ServerDialog : public QDialog
{
    Q_OBJECT
    
public:
    /**
    * @brief default constructor
    */
    explicit ServerDialog(QWidget *parent = 0);
    /**
    * @brief destructor
    */
    ~ServerDialog();
    
    /**
    * @brief accessor for the password
    */
    QString getPassword();
    /**
    * @brief
    */
    bool hasPassword();
    /**
    * @brief accessor for the selected port
    */
    int getPort();
    
    
    void setPassword(QString p);
    void enablePassword(bool p);
    void setPort(int p);
    
    void readPreferences();
    void writePrefences();
    
protected:
    void changeEvent(QEvent *e);
    
    
private:
    Ui::ServerDialog *ui;
    
    
    /**
    * pointer to the unique instance of preference manager.
    */
    PreferencesManager* m_options;
};

#endif // SERVERDIALOG_H
