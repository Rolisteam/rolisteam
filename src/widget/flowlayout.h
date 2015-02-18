/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>
#include <QStyle>


/**
  *  @brief Qt layout to manage elements as flow.
  */
class FlowLayout : public QLayout
{
public:
    /**
      * @brief constructor
      * @param parent widget
      * @param value of margin
      * @param value of horizontal space
      * @param value of vertical space
      */
    FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    /**
      * @brief constructor
      * @param value of margin
      * @param value of horizontal space
      * @param value of vertical space
      */
    FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    /**
      * @brief destructor
      */
    ~FlowLayout();
    
    /**
      * @brief add items to the layout
      */
    void addItem(QLayoutItem *item);
    /**
      * @brief accessor to the horizontal spacing
      */
    int horizontalSpacing() const;
    /**
      * @brief accessor to the vertcal spacing
      */
    int verticalSpacing() const;
    /**
      * @brief defines the direction of the expension
      * @return orientation
      */
    Qt::Orientations expandingDirections() const;
    /**
      * @brief
      */
    bool hasHeightForWidth() const;
    /**
      * @brief constructor
      */
    int heightForWidth(int) const;
    /**
      * @brief number of items
      */
    int count() const;
    /**
      * @brief constructor
      */
    QLayoutItem *itemAt(int index) const;
    /**
      * @brief constructor
      */
    QSize minimumSize() const;
    /**
      * @brief constructor
      */
    void setGeometry(const QRect &rect);
    /**
      * @brief constructor
      */
    QSize sizeHint() const;
    /**
      * @brief constructor
      */
    QLayoutItem *takeAt(int index);
    
private:
    int doLayout(const QRect &rect, bool testOnly) const;/// compute the position
    int smartSpacing(QStyle::PixelMetric pm) const; /// compute the spacing
    
    QList<QLayoutItem *> itemList;/// list of managed item
    int m_hSpace;/// horizontal space
    int m_vSpace;/// vertical space
};

#endif
