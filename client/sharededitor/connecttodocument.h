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
#ifndef CONNECTTODOCUMENT_H
#define CONNECTTODOCUMENT_H

#include <QDialog>
#include <QRegExpValidator>
#include <QUdpSocket>

#include <QListWidget>

namespace Ui {
    class ConnectToDocument;
}

class ConnectToDocument : public QDialog {
    Q_OBJECT
public:
    ConnectToDocument(QWidget *parent = 0);
    ~ConnectToDocument();

    void setName(QString name);

    QStringList previousInfo; // Public list to store previous names; addresses; ports

private:
    Ui::ConnectToDocument *ui;

    QRegExpValidator* nameValidator;
    QRegExpValidator* addressValidator;
    QRegExpValidator* portValidator;

    QUdpSocket *udpSocket;

    QList<QTimer*> timerList;
    QList<QListWidgetItem*> itemList;

    void addInfo();

    void readSettings();
    void writeSettings();

private slots:
    void dialogAccepted();
    void on_previousDocsComboBox_currentIndexChanged();
    void processPendingDatagrams();
    void timerTimedOut();
    void listWidgetItemClicked(QListWidgetItem *current);

signals:
    void connectToDocumentClicked(QStringList list);

};

#endif // CONNECTTODOCUMENT_H
