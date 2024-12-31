/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   Copyright (C) 2015 by Benoit Lagarde                                  *
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
#include <QtTest/QtTest>

#include "rwidgets/customs/diameterselector.h"
#include "rwidgets/customs/filedirchooser.h"
#include "rwidgets/customs/realslider.h"

#include "controller/view_controller/imageselectorcontroller.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include <helper.h>

class WidgetsTest : public QObject
{
    Q_OBJECT

public:
    WidgetsTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void realSliderTest();
    void fileDirChooserTest();
    void diameterSelectorTest();
    void imageSelectorTest();

private:
    std::unique_ptr<RealSlider> m_realSlider;
    std::unique_ptr<FileDirChooser> m_fileDirChooser;
    std::unique_ptr<DiameterSelector> m_diameter;
};
WidgetsTest::WidgetsTest() {}
void WidgetsTest::initTestCase()
{
    m_realSlider.reset(new RealSlider());
    m_fileDirChooser.reset(new FileDirChooser());
    m_diameter.reset(new DiameterSelector());
}

void WidgetsTest::cleanupTestCase() {}

void WidgetsTest::realSliderTest()
{
    qreal step= 0.01;
    m_realSlider->setStep(step);
    QCOMPARE(m_realSlider->step(), step);

    qreal end= 100.;
    m_realSlider->setEnd(end);
    QCOMPARE(m_realSlider->end(), end);

    qreal start= 0.;
    m_realSlider->setStart(start);
    QCOMPARE(m_realSlider->start(), start);

    m_realSlider->setRealValue(50.);

    QSignalSpy spy(m_realSlider.get(), &RealSlider::valueChanged);

    m_realSlider->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}

void WidgetsTest::fileDirChooserTest()
{
    QUrl path= QUrl::fromUserInput("/home/path/place");
    m_fileDirChooser->setUrl(path);
    QCOMPARE(m_fileDirChooser->url(), path);

    QString filter("*.png");
    m_fileDirChooser->setFilter(filter);
    QCOMPARE(m_fileDirChooser->getFilter(), filter);
}

void WidgetsTest::diameterSelectorTest()
{
    auto val= 50;
    m_diameter->setDiameter(val);
    QCOMPARE(m_diameter->getCurrentValue(), val);
}

void WidgetsTest::imageSelectorTest()
{
    auto server= Helper::initWebServer();
    ImageSelectorController ctrl(false, ImageSelectorController::All, ImageSelectorController::Square);
    ImageSelectorDialog dialog(&ctrl, nullptr, "http://127.0.0.1:9090/image/Seppun_tashime.jpg");

    QSignalSpy spy1(&ctrl, &ImageSelectorController::pixmapChanged);
    dialog.show();

    auto dlAct= dialog.findChild<QAction*>("m_downloadAct");
    dlAct->trigger();
    spy1.wait(1000);
    QCOMPARE(spy1.count(), 1);

    dialog.accept();

    auto pix= QImage::fromData(ctrl.finalImageData());
    QCOMPARE(pix.width(), pix.height());

    delete server;
}

QTEST_MAIN(WidgetsTest);

#include "tst_widgets.moc"
