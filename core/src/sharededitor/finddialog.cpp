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
#include "finddialog.h"
#include "ui_finddialog.h"

#include "enu.h"

FindDialog::FindDialog(QWidget* parent) : QDialog(parent), ui(new Ui::FindDialog)
{
    ui->setupUi(this);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FindDialog::on_findNextPushButton_clicked()
{
    QString searchString= ui->findTextEdit->toPlainText();
    Qt::CaseSensitivity sensitivity;
    if(ui->ignoreCaseCheckBox->isChecked())
    {
        sensitivity= Qt::CaseInsensitive;
    }
    else
    {
        sensitivity= Qt::CaseSensitive;
    }
    Enu::FindMode mode= Enu::Contains;
    if(ui->findModeComboBox->currentText() == "Contains")
    {
        mode= Enu::Contains;
    }
    else if(ui->findModeComboBox->currentText() == "Starts With")
    {
        mode= Enu::StartsWith;
    }
    else if(ui->findModeComboBox->currentText() == "Entire Word")
    {
        mode= Enu::EntireWord;
    }
    emit findDialogFindNext(searchString, sensitivity, ui->wrapAroundCheckBox->isChecked(), mode);
}

void FindDialog::on_findPreviousPushButton_clicked()
{
    QString searchString= ui->findTextEdit->toPlainText();
    Qt::CaseSensitivity sensitivity;
    if(ui->ignoreCaseCheckBox->isChecked())
    {
        sensitivity= Qt::CaseInsensitive;
    }
    else
    {
        sensitivity= Qt::CaseSensitive;
    }
    Enu::FindMode mode= Enu::Contains;
    if(ui->findModeComboBox->currentText() == "Contains")
    {
        mode= Enu::Contains;
    }
    else if(ui->findModeComboBox->currentText() == "Starts With")
    {
        mode= Enu::StartsWith;
    }
    else if(ui->findModeComboBox->currentText() == "Entire Word")
    {
        mode= Enu::EntireWord;
    }
    emit findDialogFindPrev(searchString, sensitivity, ui->wrapAroundCheckBox->isChecked(), mode);
}

void FindDialog::on_replaceAllPushButton_clicked()
{
    QString searchString= ui->findTextEdit->toPlainText();
    QString replaceString= ui->replaceTextEdit->toPlainText();
    Qt::CaseSensitivity sensitivity;
    if(ui->ignoreCaseCheckBox->isChecked())
    {
        sensitivity= Qt::CaseInsensitive;
    }
    else
    {
        sensitivity= Qt::CaseSensitive;
    }
    Enu::FindMode mode= Enu::Contains;
    if(ui->findModeComboBox->currentText() == "Contains")
    {
        mode= Enu::Contains;
    }
    else if(ui->findModeComboBox->currentText() == "Starts With")
    {
        mode= Enu::StartsWith;
    }
    else if(ui->findModeComboBox->currentText() == "Entire Word")
    {
        mode= Enu::EntireWord;
    }
    emit findDialogReplaceAll(searchString, replaceString, sensitivity, mode);
}

void FindDialog::on_replacePushButton_clicked()
{
    QString replaceString= ui->replaceTextEdit->toPlainText();
    emit findDialogReplace(replaceString);
}

void FindDialog::on_findReplacePushButton_clicked()
{
    QString searchString= ui->findTextEdit->toPlainText();
    QString replaceString= ui->replaceTextEdit->toPlainText();
    Qt::CaseSensitivity sensitivity;
    if(ui->ignoreCaseCheckBox->isChecked())
    {
        sensitivity= Qt::CaseInsensitive;
    }
    else
    {
        sensitivity= Qt::CaseSensitive;
    }
    Enu::FindMode mode= Enu::Contains;
    if(ui->findModeComboBox->currentText() == "Contains")
    {
        mode= Enu::Contains;
    }
    else if(ui->findModeComboBox->currentText() == "Starts With")
    {
        mode= Enu::StartsWith;
    }
    else if(ui->findModeComboBox->currentText() == "Entire Word")
    {
        mode= Enu::EntireWord;
    }
    emit findDiaalogFindReplace(searchString, replaceString, sensitivity, ui->wrapAroundCheckBox->isChecked(), mode);
}
