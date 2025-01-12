/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "model/contentmodel.h"
#include <QAbstractItemModelTester>
#include <QTest>
#include <memory>

class ContentModelTest : public QObject
{
    Q_OBJECT
public:
    ContentModelTest();

private slots:
    void init();

    void testFilteredData();

private:
    std::unique_ptr<ContentModel> m_model;
    std::unique_ptr<FilteredContentModel> m_filteredModel;
};

ContentModelTest::ContentModelTest() {}

void ContentModelTest::init()
{
    m_model.reset(new ContentModel());
    m_filteredModel.reset(new FilteredContentModel(Core::ContentType::VECTORIALMAP));

    new QAbstractItemModelTester(m_model.get());

    m_filteredModel->setSourceModel(m_model.get());
    new QAbstractItemModelTester(m_filteredModel.get());
}

void ContentModelTest::testFilteredData()
{
    m_model->appendMedia(new VectorialMapController("mapid"));

    auto i= m_model->rowCount();

    QVERIFY(i == 1);

    auto ctrl= m_model->data(m_model->index(0, 0), ContentModel::ControllerRole).value<MediaControllerBase*>();

    QVERIFY(ctrl != nullptr);

    auto vmapCtrl= dynamic_cast<VectorialMapController*>(ctrl);

    QVERIFY(vmapCtrl != nullptr);

    auto j= m_filteredModel->rowCount();

    QVERIFY(j == 1);

    auto base= m_filteredModel->contentController<VectorialMapController*>();

    QVERIFY(base.size() == 1);

    auto first= base.at(0);

    QVERIFY(first != nullptr);
}
QTEST_MAIN(ContentModelTest);

#include "tst_contentmodel.moc"
