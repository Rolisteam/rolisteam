#ifndef OOFORMAT_H
#define OOFORMAT_H
#include "FoColorName.h"
#include <QApplication>
#include <QColor>
#include <QCryptographicHash>
#include <QDebug>
#include <QDomDocument>
#include <QTextDocument>
#include <QTextTableFormat>
#include <QtCore>

static const int InterSpace= 15; /* distance from page 1 to 2 */
static const int SliderTopHeight= 28;
static const int SliderLeftHeight= 28;
static const int MaximumPages= 99;
static const int BorderShadow= 5;

static const int PageSizeID= 824;
static const int PageMarginID= 825;
static const int LayerCss2ID= 826; /* layer css on documents propritety as QString */
static const int CellBorderStyleID= 827;

static const qreal DEBUgletterspacing= 99.9;
static const int DEBUgdefaultFontPointSize= 10;

static const int CellDomelementID= 2308;

#define _LINK_COLOR_ QColor("#dc0000")

#define _NOTLINKGREB_ QString("http://qmake.net/")

/*  created */
/*
static QString bulletChar(const QTextBlock &block)
{
    Q_ASSERT(block.textList());
    QTextListFormat::Style style = block.textList()->format().style();
    if (style == QTextListFormat::ListDecimal)
        return QString::number(block.textList()->itemNumber(block));
    if (style == QTextListFormat::ListLowerAlpha)
        return QChar('a' + block.textList()->itemNumber(block));
    if (style == QTextListFormat::ListUpperAlpha)
        return QChar('A' + block.textList()->itemNumber(block));

    return bulletChar(style);
}
*/

QString ootrimmed(QString txt, const QString txttransform= QString("none"));
QTextCharFormat PreFormatChar(QTextCharFormat format= QTextCharFormat());
QTextBlockFormat DefaultMargin(QTextBlockFormat rootformats= QTextBlockFormat());
QTextCharFormat DefaultCharFormats(bool qtwritteln= false, QTextCharFormat format= QTextCharFormat());
QByteArray CatDomElement(const QDomElement e);
#ifdef _OOREADRELEASE_
QString cssGroup(const QDomElement e);
#endif
QTextFrameFormat DefaultFrameFormat();

class ReadWriteBuf
{
public:
    ReadWriteBuf(const QString xml= QString()) : d(new QBuffer())
    {
        d->open(QIODevice::ReadWrite);
        start();
        if(xml.size() > 0)
            write(xml);
    }
    bool clear()
    {
        d->write(QByteArray());
        start();
        return d->bytesAvailable() == 0 ? true : false;
    }
    qint64 write(const QString dat)
    {
        QByteArray chunk;
        chunk.append(dat);
        d->write(chunk);
        start();
        return d->bytesAvailable();
    }
    qint64 write(const QByteArray dat)
    {
        d->write(dat);
        start();
        return d->bytesAvailable();
    }
    void start() { d->seek(0); }
    bool LoadFile(const QString file)
    {
        if(clear())
        {
            QFile f(file);
            if(f.exists())
            {
                if(f.open(QFile::ReadOnly))
                {
                    d->write(f.readAll());
                    f.close();
                    start();
                    return true;
                }
            }
        }
        return false;
    }
    bool PutOnFile(const QString file)
    {
        QFile f(file);
        start();
        if(f.open(QFile::WriteOnly))
        {
            uint bi= f.write(d->readAll());
            f.close();
            start();
            return bi > 0 ? true : false;
        }
        return false;
    }
    bool isValid() { return doc.setContent(stream(), false, 0, 0, 0); }
    QDomDocument Dom() { return doc; }

    QIODevice* device() { return d; }
    QByteArray stream() { return d->data(); }
    QString toString() { return QString(d->data()); }
    QDomDocument doc;
    QBuffer* d;
};

class OOFormat : public QObject
{
    Q_OBJECT
    //
public:
    OOFormat();
    QString hashGenerator(const QString name) const;
    QTextDocument* Qdoc;
    FopColor* FoCol;
};

//
#endif // OOFORMAT_H
