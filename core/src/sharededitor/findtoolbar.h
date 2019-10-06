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
#ifndef FINDTOOLBAR_H
#define FINDTOOLBAR_H

#include <QKeyEvent>
#include <QWidget>

#include "codeeditor.h"

namespace Ui
{
    class FindToolBar;
}

class FindToolBar : public QWidget
{
    Q_OBJECT
public:
    FindToolBar(QWidget* parent= 0);
    ~FindToolBar();

    void giveFocus();

private:
    Ui::FindToolBar* ui;
    CodeEditor* codeEditor;

private slots:
    void findTriggered(QString string);

    void findNextTriggered();

    void closeButtonClicked();

protected:
    void keyPressEvent(QKeyEvent* e);

signals:
    void findAll(QString);
    void findNext(QString);
    void findPrevious(QString);
};

#endif // FINDTOOLBAR_H
