#ifndef OOREADER_H
#define OOREADER_H
#include <QDebug>
#include <QNetworkAccessManager>
#include <QtCore>
#include <QtGui>
#include <iostream>
#include <stdio.h>

#include <QDomDocument>
#include <QTextTableFormat>

#include "FoColorName.h"
#include "OOFormat.h"

QMap<QString, QByteArray> unzipstream(const QString file);

class PushDoc : public QThread
{
public:
    PushDoc(QObject* parent) : QThread(parent) { setTerminationEnabled(true); }

protected:
    void run() { exec(); }
};

class ChildImport : public QXmlStreamReader
{
public:
    ChildImport(QIODevice* device= 0);
    /* copy all subelement from reader to writer out */
    void copyDeep(QIODevice* device, QXmlStreamWriter& out);
};

class LoadGetImage : public QObject
{
    Q_OBJECT
    //
public:
    LoadGetImage(const QString nr, QUrl url_send);
    void Start();
    // inline int Htpp_id() { return Http_id; }
    inline QPixmap pics() { return resultimage; }
    QString cid;
    // int Http_id;
    QUrl url;
    QPixmap resultimage;
signals:
    void take(QString);
public slots:
    void ImageReady(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_manager;
};

class Gloader : public QThread
{
    Q_OBJECT

public:
    void Setting(QObject* parent, const QString id, QUrl url_send);

protected:
    void run();
signals:
private:
    QString cid;
    QUrl url;
    LoadGetImage* Rhttp;
    QObject* receiver;
};

class ReadWriteBuf;
class OOReader : public OOFormat
{
    Q_OBJECT
    //
public:
    enum WIDGEDEST
    {
        otextedit,
        ographicview,
        otextbrowser,
        opanel,
        ounknowwidged= 404
    };

    OOReader(const QString file= QString(), WIDGEDEST e= otextbrowser, QObject* parent= 0);
    QTextDocument* document() { return Qdoc->clone(); } /* only body */
#ifdef _OOREADRELEASE_
    QString debugStyle() { return debugline; }
#endif
    QMap<QString, QTextDocument*> option() { return lateral; } /* header footer fo region */

    enum STYLETYPE
    {
        opara= 100,          /* paragraph - block */
        ochar= 200,          /* span fragment */
        otable= 300,         /* table body footer */
        ocell= 400,          /* table cell */
        oframe= 500,         /* inline frame */
        oabsoluteframe= 600, /* floating absolute*/
        obodypage= 1000,     /* body size */
        obefore= 2000,       /* header */
        oafter= 3000,        /* footer*/
        ostart= 4000,        /* left area */
        oend= 5000,          /* right area*/
        ounknow= 10000
    };

    struct StyleInfo
    {
        StyleInfo();
        StyleInfo(const StyleInfo& d);
        StyleInfo& operator=(const StyleInfo& d);
        QString name;
        uint position;
        QTextFormat of;
        QTextCharFormat ofchar;
        OOReader::STYLETYPE type;
        QByteArray chunk;
        QString css;
        bool valid;
        bool filled;
    };

protected:
    void openStreams(QMap<QString, QByteArray> list); /* unzipped file */
    QString odtzipfile;
    int maxStyleCount;
    int sumOfBlocks;
    uint charsRead;
    WIDGEDEST construct_modus;
    int styleCurrentCount;
    QMap<int, QTextDocument*> layerlist;   /* absolute layer having y,x  */
    QMap<QString, QTextDocument*> lateral; /* header footer fo region */
    QMap<QString, StyleInfo> css2;         /* all oo style name */
    QMap<QString, QByteArray> filist;      /* all file from zip oo */

    QMap<QString, QDomElement> fontname;
    int fontTotal;
    QFont standardFont;

    QString fileHash;
    QDomDocument bodyStarter;
    bool DocInitContruct;
    bool QTWRITTELN;
    qreal FontMaxPoint;
    qreal FontMinPoint;
    int imageCurrentCount;

private:
    void registerFontDoc(const QDomElement& element);
    void styleNameSetup(const QByteArray chunk, const QString label);
    void convertStyleNameRoot(const QDomElement& element);
    bool convertBody(const QDomElement& element);
    bool iterateElements(const QDomElement e, QTextCursor& cur, const int processing);
    bool convertTable(QTextCursor& cur, const QDomElement e, const int processing, bool roottable= false);
    bool convertCellTable(const QDomElement e, QTextCursor& cur, const int processing);
    bool convertBlock(QTextCursor& cur, QDomElement e, const int processing);
    bool convertList(QTextCursor& cur, QDomElement e, const int processing, int level= 1,
        const QString classname= QString("deafult"));
    bool convertFragment(
        QTextCursor& cur, const QDomElement e, QTextCharFormat parent= QTextCharFormat(), bool HandleSpace= false);
    bool convertSpaceTag(QTextCursor& cur, const QDomElement e, QTextCharFormat parent, bool HandleSpace= false);
    void insertTextLine(
        QTextCursor& cur, QStringList line, QTextCharFormat parent= QTextCharFormat(), bool HandleSpace= false);
    bool convertFrame(QTextCursor& cur, const QDomElement e, QTextCharFormat parent, bool HandleSpace);
    bool convertImage(QTextCursor& cur, const QDomElement e, QTextCharFormat parent, bool HandleSpace= false);
    QTextFrameFormat FrameFormat(const QString name); /* format from css name class */
    /* read css2 format */
    QTextCharFormat charFormatCss2(
        const QString name, QTextCharFormat parent= QTextCharFormat(), bool HandleSpace= false);
    QPair<QTextBlockFormat, QTextCharFormat> paraFormatCss2(
        const QString name, QTextBlockFormat parent= QTextBlockFormat(), bool HandleSpace= false);

    /* formats fill */
    void TextBlockFormatPaint(const QString name, const QDomElement e);
    void TextCharFormatPaint(const QString name, const QDomElement e);
    void TextListFormatPaint(const QString name, const QDomElement e);
    void FrameImageFormatPaint(const QString name, const QDomElement e); /* image / frame */
    /* table column cell row format container + unknow */
    void UnknowFormatPaint(const QString name, const QString style_name, const QDomElement e);

    QTextBlockFormat TextBlockFormFromDom(const QDomElement e, QTextBlockFormat pf= QTextBlockFormat());
    QTextCharFormat TextCharFormFromDom(const QDomElement e, QTextCharFormat pf= QTextCharFormat());
    Qt::Alignment TagAlignElement(const QDomElement e);
    QString debugline;
signals:
    void statusRead(int, int);
    void setError(QString);
    void ready();

public slots:
    void reset(); /* abort or new file next */
    void read();
private slots:
    void ReadBody();
    void in_image(const QString imagename);
};

//
#endif // OOREADER_H
