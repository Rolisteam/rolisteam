/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#include "controller/view_controller/notecontroller.h"
#include "rwidgets/editors/noteeditor/src/notecontainer.h"
#include "rwidgets/editors/noteeditor/src/textedit.h"

#include "test_root_path.h"
#include "worker/iohelper.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QTest>
#include <helper.h>
#include <memory>

class NoteEditTest : public QObject
{
    Q_OBJECT

public:
    NoteEditTest();

private slots:
    void init();

    // void containerTest();

    void editTest();

private:
    std::unique_ptr<NoteContainer> m_container;
    std::unique_ptr<NoteController> m_ctrl;
    std::unique_ptr<TextEdit> m_edit;
};

NoteEditTest::NoteEditTest() {}

void NoteEditTest::init()
{
    m_ctrl.reset(new NoteController());
    m_container.reset(new NoteContainer(m_ctrl.get()));
    m_edit.reset(new TextEdit(m_ctrl.get()));
}

/*void NoteEditTest::containerTest()
{
    // m_container->setTitle(Helper::randomString());
}*/

void NoteEditTest::editTest()
{
    //           m_edit->
}

QTEST_MAIN(NoteEditTest);

#include "tst_noteedittest.moc"
