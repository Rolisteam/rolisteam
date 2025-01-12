/***************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                                *
 *   renaud@rolisteam.org                                                  *
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

#include <QTest>

#include "data/cleveruri.h"
#include "data/resourcesnode.h"

#include <helper.h>

class ResourceNodeTest : public QObject
{
    Q_OBJECT

public:
    ResourceNodeTest();

private slots:
    void setAndGetTest();

private:
    CleverURI m_node;
};

ResourceNodeTest::ResourceNodeTest() : m_node(Core::ContentType::NOTES) {}

void ResourceNodeTest::setAndGetTest()
{
    ResourcesNode* node= static_cast<ResourcesNode*>(&m_node);
    Helper::testAllProperties(node, {}, true);
}

QTEST_MAIN(ResourceNodeTest);

#include "tst_resourcenode.moc"
