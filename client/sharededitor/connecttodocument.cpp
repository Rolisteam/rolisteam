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
#include "connecttodocument.h"

#include <QDebug>
#include <QSettings>
#include <QTimer>

ConnectToDocument::ConnectToDocument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToDocument)
{
    ui->setupUi(this);

    ui->previousDocsComboBox->setFixedWidth(150);

    readSettings();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

    QRegExp nameRx("[a-zA-Z0-9_]*");
    nameValidator = new QRegExpValidator(nameRx, 0);
    ui->usernameLineEdit->setValidator(nameValidator);

    QRegExp addressRx("\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    addressValidator = new QRegExpValidator(addressRx, 0);
    ui->addressLineEdit->setValidator(addressValidator);

    QRegExp portRx("\\b[0-9]{0,9}\\b");
    portValidator = new QRegExpValidator(portRx, 0);
    ui->portLineEdit->setValidator(portValidator);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(45321);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listWidgetItemClicked(QListWidgetItem*)));
}

ConnectToDocument::~ConnectToDocument()
{
    writeSettings();

    delete ui;
}

void ConnectToDocument::setName(QString name)
{
    ui->usernameLineEdit->setText(name);
}

void ConnectToDocument::addInfo()
{
    QString newItem = QString("%1@%2:%3")
                      .arg(ui->usernameLineEdit->text())
                      .arg(ui->addressLineEdit->text())
                      .arg(ui->portLineEdit->text());
    if (!previousInfo.contains(newItem)) {
        if (previousInfo.size() > 4) {
            previousInfo.removeFirst();
            ui->previousDocsComboBox->removeItem(1);
        }
        ui->previousDocsComboBox->addItem(newItem);
        previousInfo.append(newItem);
    }
}

void ConnectToDocument::readSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    int length = settings.beginReadArray("infoList");
    for (int i = 0; i < length; ++i) {
        settings.setArrayIndex(i);
        previousInfo.append(settings.value("allInfo").toString());
        ui->previousDocsComboBox->addItem(settings.value("allInfo").toString());
    }
    settings.endArray();
}

void ConnectToDocument::writeSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    settings.beginWriteArray("infoList");
    for (int i = 0; i < previousInfo.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("allInfo", previousInfo.value(i));
    }
    settings.endArray();
}

void ConnectToDocument::dialogAccepted()
{
    addInfo();
    QStringList list;
    list.append(ui->usernameLineEdit->text());
    list.append(ui->addressLineEdit->text());
    list.append(ui->portLineEdit->text());
    emit connectToDocumentClicked(list);
}

void ConnectToDocument::on_previousDocsComboBox_currentIndexChanged()
{
    if (ui->previousDocsComboBox->currentText() != "New...") {
        QString info = ui->previousDocsComboBox->currentText();
        QRegExp rx = QRegExp("(\\w+)@([0-9\\.]+):(\\d+)");
        if (info.contains(rx)) {
            ui->usernameLineEdit->setText(rx.cap(1).toLatin1());
            ui->addressLineEdit->setText(rx.cap(2).toLatin1());
            ui->portLineEdit->setText(rx.cap(3).toLatin1());
        }
    }
}

void ConnectToDocument::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        QRegExp rx = QRegExp("([a-zA-Z0-9_\\.]*)@([0-9\\.]+):(\\d+)");
        QString data = datagram.data();
        if (data.contains(rx)) {
            QString data = QString(rx.cap(1) + ", " + rx.cap(2) + ":" + rx.cap(3));
            for (int i = 0; i < itemList.size(); i++) {
                if (itemList.at(i)->text() == data) {
                    timerList.at(i)->start(5000);
                    return; // item already exists in the list, nothing to do here
                }
            }
            QListWidgetItem *item = new QListWidgetItem(data, ui->listWidget);
            ui->listWidget->insertItem(0, item);

            QTimer *timer = new QTimer(this);
            timer->start(5000); // five second timeout
            connect(timer, SIGNAL(timeout()), this, SLOT(timerTimedOut()));
            itemList.prepend(item);
            timerList.prepend(timer);
        }
    }
}

void ConnectToDocument::timerTimedOut()
{
    QTimer *timer = qobject_cast<QTimer *>(sender());
    timer->stop();
    int index = 0;
    for (int i = 0; i < timerList.size(); i++) {
        if (timer == timerList.at(i)) {
            index = i;
        }
    }
//    qDebug() << "removing item";
    ui->listWidget->takeItem(index); // This is guaranteed to be at the same index as the itemList/timerList
    itemList.removeAt(index);
    timerList.removeAt(index);
}

void ConnectToDocument::listWidgetItemClicked(QListWidgetItem *current)
{
    QString text = current->text();
//    qDebug() << "Clicked: " << text;
    QRegExp rx = QRegExp("([a-zA-Z0-9_\\.]*),\\s([0-9\\.]+):(\\d+)");
    if (text.contains(rx)) {
        ui->addressLineEdit->setText(rx.cap(2));
        ui->portLineEdit->setText(rx.cap(3));
    }
}






