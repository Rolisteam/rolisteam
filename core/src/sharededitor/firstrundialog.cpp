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
#include "firstrundialog.h"
//#include "ui_firstrundialog.h"

#include "utilities.h"

FirstRunDialog::FirstRunDialog(QWidget* parent) : QDialog(parent), ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
    if(Utilities::getSystem() == "Mac")
    {
        ui->labelPc->hide();
    }
    else
    {
        ui->labelMac->hide();
    }
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::dialogAccepted()
{
    hide();
}
