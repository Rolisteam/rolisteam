/*
** Copyright (C) 2013 Jiří Procházka (Hobrasoft)
** Contact: http://www.hobrasoft.cz/
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file is under the terms of the GNU Lesser General Public License
** version 2.1 as published by the Free Software Foundation and appearing
** in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the
** GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
*/

#ifndef _MRICHTEXTEDIT_H_
#define _MRICHTEXTEDIT_H_

#include "rwidgets_global.h"
#include <QPointer>
#include <QWidget>
#include <QTextListFormat>
#include <QTextList>

namespace Ui
{
class MRichTextEdit;
}
/**
 * @Brief A simple rich-text editor
 */
class RWIDGET_EXPORT MRichTextEdit : public QWidget
{
    Q_OBJECT
public:
    MRichTextEdit(QWidget* parent= 0);
    ~MRichTextEdit();

    QString toPlainText() const;
    QString toHtml() const;
    QTextDocument* document();
    QTextCursor textCursor() const;
    void setTextCursor(const QTextCursor& cursor);

public slots:
    void setText(const QString& text);

protected slots:
    void setPlainText(const QString& text);
    void setHtml(const QString& text);
    void textRemoveFormat();
    void textRemoveAllFormat();
    void textBold();
    void textUnderline();
    void textStrikeout();
    void textItalic();
    void textSize(const QString& p);
    void textLink(bool checked);
    void textStyle(int index);
    void textFgColor();
    void textBgColor();
    void listBullet(bool checked);
    void listOrdered(bool checked);
    void slotCurrentCharFormatChanged(const QTextCharFormat& format);
    void slotCursorPositionChanged();
    void slotClipboardDataChanged();
    void increaseIndentation();
    void decreaseIndentation();
    void insertImage();
    void textSource();

protected:
    void mergeFormatOnWordOrSelection(const QTextCharFormat& format);
    void fontChanged(const QFont& f);
    void fgColorChanged(const QColor& c);
    void bgColorChanged(const QColor& c);
    void list(bool checked, QTextListFormat::Style style);
    void indent(int delta);
    void focusInEvent(QFocusEvent* event);

    QStringList m_paragraphItems;
    int m_fontsize_h1;
    int m_fontsize_h2;
    int m_fontsize_h3;
    int m_fontsize_h4;

    enum ParagraphItems
    {
        ParagraphStandard= 0,
        ParagraphHeading1,
        ParagraphHeading2,
        ParagraphHeading3,
        ParagraphHeading4,
        ParagraphMonospace
    };

    QPointer<QTextList> m_lastBlockList;
    std::unique_ptr<Ui::MRichTextEdit> m_ui;
};

#endif
