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
#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>

namespace Ui
{
    class FirstRunDialog;
}

class FirstRunDialog : public QDialog
{
    Q_OBJECT
public:
    FirstRunDialog(QWidget* parent= 0);
    ~FirstRunDialog();

private:
    Ui::FirstRunDialog* ui;

private slots:
    void dialogAccepted();
};

#endif // FIRSTRUNDIALOG_H
