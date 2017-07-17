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
#include "findtoolbar.h"
#include "ui_findtoolbar.h"

FindToolBar::FindToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindToolBar)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(findTriggered(QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
}

FindToolBar::~FindToolBar()
{
    delete ui;
}

void FindToolBar::giveFocus()
{
    ui->lineEdit->setFocus();
    ui->lineEdit->setText("");
}

void FindToolBar::findTriggered(QString string)
{
    if (string.length() > 2) {
        emit findAll(string);
    }
}

void FindToolBar::findNextTriggered()
{
    emit findNext(ui->lineEdit->text());
}

void FindToolBar::closeButtonClicked()
{
    this->hide();
}

void FindToolBar::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        closeButtonClicked();
    }
    else if (ui->lineEdit->hasFocus() && e->key() == Qt::Key_Return && e->modifiers() != Qt::ShiftModifier) {
        emit findNext(ui->lineEdit->text());
    }
    else if (ui->lineEdit->hasFocus() && e->key() == Qt::Key_Return && e->modifiers() == Qt::ShiftModifier) {
        emit findPrevious(ui->lineEdit->text());
    }
}
