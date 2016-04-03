/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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

#ifndef CONNECTIONRETRYDIALOG_H
#define CONNECTIONRETRYDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class ConnectionRetryDialog;
}
/**
 * @brief The ConnectionRetryDialog class
 */
class ConnectionRetryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConnectionRetryDialog(QWidget *parent = 0);
    ~ConnectionRetryDialog();

public slots:
    void resetCounter();
    void startTimer();
    void setTimeOut(int );

private slots:
    void decreaseCounter();


signals:
    void tryConnection();
    
private:
    Ui::ConnectionRetryDialog *ui;
    int m_timeoutValue;
    int m_counter;
    QTimer* m_timer;
    QString m_msg;
};

#endif // CONNECTIONRETRYDIALOG_H
