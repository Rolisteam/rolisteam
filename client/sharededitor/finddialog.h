/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

#include "enu.h"
#include "utilities.h"
// clazy:skip
namespace Ui
{
    class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT
public:
    FindDialog(QWidget* parent= nullptr);
    ~FindDialog();

protected:
    void changeEvent(QEvent* e);

private:
    Ui::FindDialog* ui;

private slots:
    void on_findNextPushButton_clicked();
    void on_findPreviousPushButton_clicked();
    void on_replaceAllPushButton_clicked();
    void on_replacePushButton_clicked();
    void on_findReplacePushButton_clicked();

signals:
    void findDialogFindNext(QString str, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode findMode);
    void findDialogFindPrev(QString str, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode findMode);
    void findDialogReplaceAll(QString find, QString replace, Qt::CaseSensitivity sensitivity, Enu::FindMode findMode);
    void findDialogReplace(QString replace);
    void findDiaalogFindReplace(
        QString find, QString replace, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode findMode);
};

#endif // FINDDIALOG_H
