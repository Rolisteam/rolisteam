#include "OOReader.h"
//
/*  Save file as OOReader.cpp  */
/*  incomming class name OOReader */
//


#include "GZipWriter.h"
#include "GZipReader.h"



/*                    file name and path , his data */
QMap<QString,QByteArray> unzipstream( const QString file )
{
	/*
	  #include "qzipreader_p.h"
	*/
	QMap<QString,QByteArray> ooFile;
	CZIP::QZipReader unzip(file,QIODevice::ReadOnly);
	QList<CZIP::QZipReader::FileInfo> list = unzip.fileInfoList();
	CZIP::QZipReader::FileInfo fi;
	if (list.size() > 0 && unzip.exists()) {
		for (int l = 0; l < list.size(); l++) {
			fi = list.at(l);
			if (fi.size > 0) {
				ooFile.insert(fi.filePath,unzip.fileData(fi.filePath));
			}
			/////////qDebug() << "### file  " << fi.filePath << fi.size;
		}

		unzip.close();
	}
	return ooFile;

	/*  usage
	QString txtlog;
	QMap<QString,QByteArray>  filist = unzipstream("giallo.odt");
	QMapIterator<QString,QByteArray> i(filist);
	    while (i.hasNext()) {
	        i.next();
	        qDebug() << "### name---------- " << i.key();
	        txtlog.append(i.key());
	    }
	QByteArray base = filist["content.xml"];
	if (base.size() > 0) {
	   txtlog.append(QString(base));
	}
	*/
}











/* image get remote */


LoadGetImage::LoadGetImage( const QString nr , QUrl url_send  )
		: QHttp(url_send.host(),QHttp::ConnectionModeHttp ,80)
{
	url = url_send;
	cid = nr;
	setHost(url_send.host() , 80);
}

void LoadGetImage::Start()
{
	const QString METHOD =  "GET";
	const QString agent = QString("Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)");
	header.setRequest(METHOD,url.path(),1,1);
	header.setValue("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
	header.setValue("Host",url.host());
	header.setValue("User-Agent",agent);
	connect(this, SIGNAL(done(bool)), this , SLOT(ImageReady(bool)));
	Http_id = request(header,0,0);
}

void LoadGetImage::ImageReady( bool error )
{
	if (!error) {
		resultimage.loadFromData(readAll());
		if (!resultimage.isNull()) {
			emit take(cid);
		}


	}
}


void Gloader::Setting( QObject *parent , const QString  id , QUrl url_send )
{
	receiver = parent;
	cid = id;
	url = url_send;
	setTerminationEnabled(true);
}

void Gloader::run()
{
	Rhttp = new LoadGetImage(cid,url);
	connect(Rhttp, SIGNAL(take(QString)), receiver , SLOT(in_image(QString)));
	Rhttp->Start();
	exec();
}



/* -------------------------------------------------------------------------------------------------------------------------*/
/* ----------------------------------------------- reader --------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------------------------------------------------*/

#include <QDomDocument>

OOReader::OOReader( const QString file , OOReader::WIDGEDEST e , QObject* parent )
		: OOFormat(),maxStyleCount(0),styleCurrentCount(0),charsRead(),QTWRITTELN(false),
		construct_modus(e),sumOfBlocks(0),fileHash("250186"),fontTotal(0)
{

	if (qApp->thread() != QThread::currentThread()) {
		qDebug() << "### Heya speed other  QThread as Apps....";
	}
	odtzipfile = file;
	reset();
}


void OOReader::read()
{
    Qdoc = new QTextDocument();  /* root document main */
    standardFont = QApplication::font();
    
    
    
	QFileInfo fi(odtzipfile);
	if (!fi.exists()) {
		emit ready();
		return;
	}
	reset();
	/* destrox delete class befor init other file */
	if (fileHash == hashGenerator( odtzipfile )) {
		emit ready();
		return;
	}
	fileHash = hashGenerator( odtzipfile  );
	filist = unzipstream(odtzipfile);
	qDebug() << "### open file id  " << fileHash;
	openStreams( filist );
}

void OOReader::in_image( const QString imagename )
{
	LoadGetImage *ht = qobject_cast<LoadGetImage *>(sender());
	if (ht) {
		QPixmap  imagen = ht->pics();
		qDebug() << "### remote image incomming " << imagename;
		Qdoc->addResource(QTextDocument::ImageResource,QUrl(imagename),imagen);
		emit ready();
	}
}



void OOReader::openStreams( QMap<QString,QByteArray> list )
{
	reset();
	filist = list;
	QByteArray content = filist["content.xml"];
	QByteArray styles = filist["styles.xml"];
	if (content.size() > 0) {
		const QString maindoc = QString(content);
		const int sumBlock = maindoc.count("<text:p");
		if (sumBlock == 0) {
			return;
		}
		else {
			sumOfBlocks = sumBlock;
		}
		styleNameSetup(content,QString("text:style-name"));
		styleNameSetup(content,QString("table:style-name"));
		styleNameSetup(content,QString("draw:style-name"));
	}
	if (styles.size() > 0) {
		/* creator ooo default */
		styleNameSetup(styles,QString("text:style-name"));
		styleNameSetup(styles,QString("table:style-name"));
		styleNameSetup(styles,QString("draw:style-name"));
	}
	else {
		/* creator by QT version 4.5*/
		QTWRITTELN = true;
	}
	if (maxStyleCount < 1) {
		emit ready();
		return;
	}
	if (css2.size() != maxStyleCount) {
		emit ready();
		return;
	}
	qDebug() << "### style read from id " << fileHash;
	qDebug() << "### css2.size() -> " << maxStyleCount << "--------------------------------------------------------<";
	qDebug() << "### total block count to progress -----> " << sumOfBlocks;

	/* init progress read loading */
	emit statusRead(0,sumOfBlocks);
	/* fill css2.size() style before  draw text */
	if (styles.size() > 0) {
		ReadWriteBuf *Sbuf = new ReadWriteBuf();
		Sbuf->write(styles);
	}

	if (content.size() > 0) {
		ReadWriteBuf *Mbuf = new ReadWriteBuf();
		Mbuf->write(content);
		/////////QXmlStreamReader oMread(Mbuf->device());
		////////oMread.setNamespaceProcessing(false);

		if (Mbuf->isValid()) {
			bodyStarter = Mbuf->Dom();
			const QDomElement root = Mbuf->Dom().documentElement();
            QDomElement fonti = root.firstChildElement("office:font-face-decls");
            registerFontDoc(fonti);
            /* font declare at first */
            
			QDomElement style = root.firstChildElement("office:automatic-styles");
			convertStyleNameRoot(style);
		}

		/* time to set style */
	}
	DocInitContruct = false;
	ReadBody();
}




void OOReader::ReadBody()
{
	if (DocInitContruct) {
		emit ready();
		return;  /* only reset to rebuild */
	}
	styleCurrentCount = 0;
	DocInitContruct = true;
	const QDomElement root = bodyStarter.documentElement();
	QDomElement bodydoc = root.firstChildElement("office:body");
	bool success = convertBody(bodydoc);
	if (success) {
		emit statusRead(sumOfBlocks,sumOfBlocks);
	}
	else {
		/* destroy progress ! */
		emit statusRead(sumOfBlocks,sumOfBlocks);
		emit setError(tr("Unable to convert document body!"));
	}
#ifdef _OOREADRELEASE_   ///// QMap<QString,StyleInfo> css2;
	debugline = "";
	QMapIterator<QString,StyleInfo> i(css2);
	while (i.hasNext()) {
		i.next();
		StyleInfo style = i.value();
		debugline.append(QString("Name: %1, %2\n").arg(i.key()).arg(style.css));
	}
#endif
	emit ready();
    qDebug() << "### end reader action........................................................... ";
}

bool OOReader::convertBody( const QDomElement &element )
{
	QTextCursor cursor(Qdoc);
	cursor.movePosition(QTextCursor::End);
	styleCurrentCount = 0;
	QDomElement bodydoc = element.firstChildElement("office:text");
	QDomElement child = bodydoc.firstChildElement("text:p");
	while ( !child.isNull() ) {

		///////////////////////qDebug() << "### body root loop on -> " << child.tagName();

		if ( child.tagName() == QLatin1String( "text:p" ) ) {
			convertBlock(cursor,child,styleCurrentCount);
			styleCurrentCount++;
			emit statusRead(styleCurrentCount,sumOfBlocks);
			//////qDebug() << "### block root Nr.---" << styleCurrentCount << sumOfBlocks;
			///////QCoreApplication::processEvents();
		}
		else if ( child.tagName() == QLatin1String( "text:list" ) ) {
			/* span element */
			convertList(cursor,child,styleCurrentCount,1);
			qDebug() << "### list root Nr.---" << styleCurrentCount << sumOfBlocks;
			styleCurrentCount++;
			emit statusRead(styleCurrentCount,sumOfBlocks);
			//////////QCoreApplication::processEvents();
		}
		else if ( child.tagName() == QLatin1String( "table:table" ) ) {
			convertTable(cursor,child,styleCurrentCount,true);
		}
		else if (child.isText()) {
			cursor.insertText(child.nodeValue()+"|TEXT_NODE_ROOT|");
		}
		child = child.nextSiblingElement();
		/////////convertTable( QTextCursor &cur , QDomElement e  , const int processing )
	}
	return true;
	///// cursor.movePosition(QTextCursor::End);
}


bool OOReader::iterateElements( const QDomElement e ,   QTextCursor &cursor  , const int processing  )
{
	int loppiter = -1;
	QDomElement child = e.firstChildElement();
	while ( !child.isNull() ) {
		if ( child.tagName() == QLatin1String( "text:p" ) ) {
			loppiter++;
			styleCurrentCount++;
			convertBlock(cursor,child,loppiter);

			emit statusRead(styleCurrentCount,sumOfBlocks);
		}
		else if ( child.tagName() == QLatin1String( "text:list" ) ) {
			/* span element */
			convertList(cursor,child,styleCurrentCount,1);
			styleCurrentCount++;
			emit statusRead(styleCurrentCount,sumOfBlocks);
		}
		else if ( child.tagName() == QLatin1String( "table:table" ) ) {
			convertTable(cursor,child,styleCurrentCount,false);
		}
		else if (child.isText()) {
			cursor.insertText(child.nodeValue()+"|TEXT_NODE_ROOT|");
		}
		child = child.nextSiblingElement();
	}
	return true;
}



bool OOReader::convertList( QTextCursor &cur , QDomElement e  , const int processing , int level , const QString classname )
{
	qDebug() << "### convertList begin " << processing << " name " << e.tagName() << " l." << level;
	QString sname = classname;
	if (level == 1) {
		sname = e.attribute ("text:style-name");
	}

	if (sname.size() < 1) {
		qWarning() << "### No valid name on QTextListFormat name " << sname;
		return false;
	}
	StyleInfo blockformat;
	QTextListFormat ulinit;

	if (css2[sname].valid) {
		blockformat = css2[sname];
		ulinit = blockformat.of.toListFormat();
	}
	else {
		qWarning() << "### No valid QTextListFormat name  on css2 list " << sname;
		return false;
	}

	ulinit.setIndent ( level );
	/* insert first block to take format and create list */
	QDomElement ul_blck = e.firstChildElement("text:list-item");  /* grab next from this */

	QDomElement nextlevel = ul_blck.firstChildElement("text:list");
	if (!nextlevel.isNull()) {
		return convertList(cur,nextlevel,styleCurrentCount,level + 1,sname);
	}

	QDomElement firstpara = ul_blck.firstChildElement("text:p");
	const int CCpos = cur.position();
	QTextCharFormat spanFor = DefaultCharFormats(QTWRITTELN);
	QTextBlockFormat paraFormat = DefaultMargin();
	if (!ul_blck.isNull() && !firstpara.isNull()) {
		cur.insertBlock();   /* make first block  to convert to list format */
		cur.beginEditBlock();
		cur.setBlockFormat(paraFormat);
		cur.setCharFormat(spanFor);
		styleCurrentCount++;  /* count block to all doc */
		if (convertBlock(cur,firstpara,0)) {
			cur.endEditBlock();
		}
	}


	int lisumm = 1;
	QTextList *Uls = cur.createList( ulinit );
	Uls->add( cur.block() );
	cur.endEditBlock();
	return true;
}

QPair<QTextBlockFormat,QTextCharFormat> OOReader::paraFormatCss2( const QString name , QTextBlockFormat parent , bool HandleSpace  )
{
	QTextBlockFormat paraFormat = DefaultMargin();
	paraFormat.merge(parent);
	QTextCharFormat charFormat = DefaultCharFormats(QTWRITTELN);
	if (css2[name].valid) {
		StyleInfo format = css2[name];
		charFormat.merge(format.ofchar);
		paraFormat.merge(format.of.toBlockFormat());
	}
	Q_ASSERT(paraFormat.isValid());
	Q_ASSERT(charFormat.isValid());

	return qMakePair(paraFormat,charFormat);
}

QTextCharFormat OOReader::charFormatCss2( const QString name , QTextCharFormat parent , bool HandleSpace  )
{
	QTextCharFormat base = DefaultCharFormats(QTWRITTELN);
	base.merge(parent);
	if (css2[name].valid && parent.isValid()) {
		StyleInfo format = css2[name];
		base.merge(format.ofchar);
	}
	Q_ASSERT(base.isValid());
	return base;
}


bool OOReader::convertBlock( QTextCursor &cur , QDomElement e  , const int processing )
{
	const QString sname = e.attribute ("text:style-name","default");
	StyleInfo blockformat;
	QTextBlockFormat paraFormat = paraFormatCss2(sname).first;
	QTextCharFormat spanFor = paraFormatCss2(sname).second;
	if (processing == 0) {
		cur.beginEditBlock();
	}
	else {
		cur.insertBlock();
		cur.beginEditBlock();
	}
	if (!paraFormat.isValid () ) {
		paraFormat = DefaultMargin();
	}
	cur.setBlockFormat(paraFormat);
	cur.setCharFormat(spanFor);
	QDomNode child = e.firstChild();
	while ( !child.isNull() ) {
		if ( child.isElement() )  {
			const QDomElement es = child.toElement();
			///////////qDebug() << "### para loop ->" << es.tagName();
			if ( es.tagName() == QLatin1String( "text:span" )) {
				convertFragment(cur,es,spanFor);
			}
			if ( es.tagName() == QLatin1String( "text:a" )) {
				spanFor.setAnchor(true);
				spanFor.setAnchorHref(es.attribute ("xlink:href",_NOTLINKGREB_));
				spanFor.setForeground(QBrush(_LINK_COLOR_));
				spanFor.setToolTip ( es.attribute ("xlink:href",_NOTLINKGREB_) );
				cur.setCharFormat(spanFor);
				convertFragment(cur,es.firstChildElement(),spanFor);

			}
			else if (es.tagName() == QLatin1String( "text:line-break" )) {
				cur.insertText(QString(QChar::LineSeparator));
			}
			else if (es.tagName() == QLatin1String( "text:s" )) {
				convertSpaceTag(cur,es,spanFor);
			}
			else if (es.tagName() == QLatin1String( "draw:frame" )) {
				convertFrame(cur,es,spanFor,false);
			}
			else if (es.tagName() == QLatin1String( "text:soft-page-break" )) {
				/* page breack before or after !!!! */
				paraFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
				cur.setBlockFormat(paraFormat);
			}
		}
		else if (child.isText()) {
			insertTextLine(cur,child.nodeValue().split("\n"),spanFor);
		}
		else if (child.isCDATASection()) {
			QString pretag = Qt::escape(child.nodeValue());
			pretag.prepend("<pre>");
			pretag.append("</pre>");
			QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(pretag);
			cur.setCharFormat ( PreFormatChar() );
			cur.insertFragment(fragment);
			cur.setCharFormat ( PreFormatChar() );
		}
		child = child.nextSibling();
	}
	if (e.text().isEmpty()) {
		cur.insertText(QString(QChar::Nbsp));
	}
	cur.mergeCharFormat ( spanFor );
	cur.endEditBlock();
	return true;
}

bool OOReader::convertFrame( QTextCursor &cur , const QDomElement e , QTextCharFormat parent ,  bool HandleSpace )
{
	if (e.isNull()) {
		return false;
	}
    const QString name = e.attribute ("draw:style-name");
	const qreal width = Unit(e.attribute("svg:width"));
	const qreal height = Unit(e.attribute("svg:height"));
	const qreal poX = Unit(e.attribute("svg:x"));
	const qreal poY = Unit(e.attribute("svg:y"));
	const int zindex = e.attribute("draw:z-index").toInt();
    
    QTextFrameFormat fox;
    if (css2[name].valid) {
		fox = css2[name].of.toFrameFormat();
    }
    fox.setWidth ( width );
	fox.setHeight( height );
    Q_ASSERT(fox.isValid());
    
	const int lastposition = cur.position();
	QTextCursor cur2(Qdoc);
	cur2.setPosition(lastposition);

	QDomElement img = e.firstChildElement("draw:image");
	QDomElement txtframe = e.firstChildElement("draw:text-box");
	if (!img.isNull()) {
		return convertImage(cur,e,parent ,HandleSpace);
	}
	if (!txtframe.isNull()) {
		QTextFrame  *floatframe = cur2.insertFrame(fox);
		QTextCursor  cur3 = floatframe->firstCursorPosition();
		cur.movePosition(QTextCursor::End);
		return iterateElements(txtframe,cur3,0);
	}

	return false;
}

bool OOReader::convertImage( QTextCursor &cur , const QDomElement e , QTextCharFormat parent ,  bool HandleSpace )
{
	if (e.isNull()) {
		return false;
	}
	if (e.tagName() != QLatin1String( "draw:frame" ) ) {
		return false;
	}
	QDomElement img = e.firstChildElement("draw:image");
	if (img.isNull()) {
		return false;
	}
	imageCurrentCount++;
	QDateTime timer1( QDateTime::currentDateTime() );
	/* on oo format user dont see image name randomise it */
	const QString  TimestampsMs = QString("%1-%2%3-graphic-frame").
	                              arg(timer1.toTime_t()).arg(timer1.toString("zzz")).
	                              arg(imageCurrentCount);

//////////////qDebug() << "### Init Frame image ID  " << TimestampsMs;

	QImage drawimage;
	const QString sname = e.attribute ("draw:style-name");
	const QString uri = img.attribute ("xlink:href");
	const QByteArray domdd = css2[sname].chunk;
	QUrl turi(uri);
	if (domdd.size() > 0 || QTWRITTELN) {
		/* load dom to rotate image mirror or saved on style load  qt not save image on style
		not need .... */
		/* default image */
		if (uri.startsWith("Pictures/")) {
			drawimage.loadFromData (filist[uri]);
		}
		else if (turi.scheme() == "file") {
			QFileInfo fi(turi.toLocalFile());
			if (fi.exists () ) {
				drawimage = QImage(fi.absoluteFilePath());
			}
		}
		else if (turi.scheme() != "file") {
			/* remote url take to draw ? and TimestampsMs is url */
			Gloader *grephttp = new Gloader();
			grephttp->Setting(this,TimestampsMs,turi);
			grephttp->start(QThread::LowPriority);
		}

		if (!drawimage.isNull()) {
			Qdoc->addResource(QTextDocument::ImageResource,QUrl(TimestampsMs),drawimage);
		}

		QTextImageFormat format;
		format.setName( TimestampsMs );
		if (Unit(e.attribute( "svg:height")) > 0) {
			format.setHeight (Unit(e.attribute( "svg:height")));
		}
		if (Unit(e.attribute( "svg:width")) > 0) {
			format.setWidth ( Unit(e.attribute( "svg:width")) );
		}
		format.setToolTip(TimestampsMs);
		cur.insertImage( format );
		return true;
	}
	else {
		qWarning() << "### Image name style not load ";
		return false;
	}


}





bool OOReader::convertSpaceTag( QTextCursor &cur , const QDomElement e , QTextCharFormat parent ,  bool HandleSpace )
{
	if (e.isNull()) {
		return false;
	}
	if (e.tagName() != QLatin1String( "text:s" ) ) {
		return false;
	}
	const int space = e.attribute( "text:c").toInt();
	QString spacestring = "";
	for (int i = 0; i < space; ++i) {
		if (!QTWRITTELN) {
			/////spacestring.append(QString("-"));
			spacestring.append(QString(QChar::Nbsp));
		}
		else {
			spacestring.append(QString(QChar::Nbsp));
		}
	}
	if (spacestring.isEmpty()) {
		cur.insertText(QString(QChar::Nbsp),parent);
	}
	else {
		cur.insertText(spacestring,parent);
	}
	return true;

}

bool OOReader::convertFragment( QTextCursor &cur , const QDomElement e , QTextCharFormat parent ,  bool HandleSpace )
{
	if (e.isNull()) {
		return false;
	}
	QTextCharFormat styleroot = DefaultCharFormats(QTWRITTELN);
	styleroot.merge(parent);
	cur.setCharFormat(styleroot);
	if ( e.tagName() == QLatin1String( "text:span" ) ) {
		styleroot.merge(charFormatCss2(e.attribute ("text:style-name","default"),parent,HandleSpace));
		cur.setCharFormat(styleroot);
		if (e.text().isEmpty()) {
			cur.insertText(QString(QChar::Nbsp),parent);
		}
	}
	//////////////qDebug() << "### convertFragment  fragment tag in  ->" << e.tagName();

	QDomNode child = e.firstChild();
	while ( !child.isNull() ) {

		QDomNode subchild = child.firstChild();
		if (!subchild.isNull() && subchild.isElement() ) {
			convertFragment(cur,subchild.toElement(),styleroot,HandleSpace);
		}

		if ( child.isElement() )  {
			const QDomElement es = child.toElement();
			////////qDebug() << "### loop fragment  tag ->" << es.tagName();

			if (es.tagName() == QLatin1String( "text:line-break" )) {
				cur.insertText(QString(QChar::LineSeparator));
			}
			else if ( es.tagName() == QLatin1String( "text:span" ) ) {
				convertFragment(cur,es,styleroot,HandleSpace);
			}
			else if ( es.tagName() == QLatin1String( "draw:frame" ) ) {
				convertImage(cur,es,parent,HandleSpace);
			}
			else if ( es.tagName() == QLatin1String( "text:s" ) ) {
				convertSpaceTag(cur,es,parent,HandleSpace);
			}
			else if ( es.tagName() == QLatin1String( "text:a" )) {
				styleroot.setAnchor(true);
				styleroot.setAnchorHref(es.attribute ("xlink:href",_NOTLINKGREB_));
				styleroot.setForeground(QBrush(_LINK_COLOR_));
				styleroot.setToolTip ( es.attribute ("xlink:href",_NOTLINKGREB_) );
				cur.setCharFormat(styleroot);
				convertFragment(cur,es.firstChildElement(),styleroot);
			}


		}
		else if (child.isText()) {
			insertTextLine(cur,child.nodeValue().split("\n"),styleroot);
		}
		child = child.nextSibling();
	}


	return true;
}

void OOReader::insertTextLine( QTextCursor &cur , QStringList line , QTextCharFormat parent ,  bool HandleSpace )
{

	Q_ASSERT(parent.isValid());


	for (int i = 0; i < line.size(); ++i) {
		if (HandleSpace) {
			/* trim fo standard fop namenspace text */
			cur.insertText(ootrimmed(line.at(i)),parent);
		}
		else {
			cur.insertText(line.at(i),parent);
		}
		if (i > 0) {
			cur.insertText(QString(QChar::Nbsp),parent);
		}
	}
	/* span no text */
	if (line.size() == 0) {
		cur.insertText(QString(QChar::Nbsp),parent);
	}
	/* log text here */

}

void OOReader::reset()
{
	FontMaxPoint = 0.0;
	FontMinPoint = 20.0;
	imageCurrentCount = 0;
	filist.clear();
	css2.clear();
	lateral.clear();
	maxStyleCount = 0;
	styleCurrentCount = 0;
	bodyStarter = QDomDocument();
	DocInitContruct = false;
    fontname.clear();
    fontTotal = 0;
}


void OOReader::styleNameSetup(  const QByteArray chunk   , const QString label  )
{
	const QString searchPara = QString("%1=").arg(label);
	QRegExp expression( searchPara + "[\"\'](.*)[\"\']", Qt::CaseInsensitive );
	expression.setMinimal(true);
	QString valueName = "";
	StyleInfo default_style;
	int iPosition = 0;
	while ( (iPosition = expression.indexIn( QString(chunk) , iPosition )) != -1 ) {
		valueName = expression.cap( 0 );
		valueName = valueName.mid(searchPara.size() + 1,valueName.size()  - searchPara.size() - 2 );
		const QString name = valueName;
		if (!css2[name].valid) {
			default_style.valid = true;
			default_style.name = name;
			css2.insert(name,default_style);
			maxStyleCount++;
		}
		iPosition += expression.matchedLength();

	}
}







Qt::Alignment OOReader::TagAlignElement(const QDomElement e )
{
	QString value;
	QString FAli0 = e.attribute( "fo:text-align");
	QString FAli1 = qMax(e.attribute( "fo:align") , e.attribute( "table:align"));
	if (!FAli0.isEmpty()) {
		value = FAli0.simplified().toLower();
	}
	if (!FAli1.isEmpty()) {
		value = FAli1.simplified().toLower();
	}
	if ( e.attribute( "fo:vertical-align") == "middle" || e.attribute( "fo:display-align") == "center" ) {
		return Qt::AlignVCenter;
	}
	if (value.size() < 1) {
		return Qt::AlignLeft;
	}
	else if (value == "center" ) {
		return Qt::AlignCenter;
	}
	else if (value == "right" || value == "end" ) {
		return Qt::AlignRight;
	}
	else if (value == "justify" ) {
		return Qt::AlignJustify;
	}
	else if (value == "inherit" || value == "inside" ) {
		return Qt::AlignAbsolute;
	}
	else if (value == "start" || value == "left" ) {
		return Qt::AlignLeft;
	}
	else {
		return Qt::AlignLeft;
	}
}





OOReader::StyleInfo::StyleInfo(const StyleInfo &d )
{
	operator=(d);
}

OOReader::StyleInfo& OOReader::StyleInfo::operator=(const StyleInfo &d)
{
	position = d.position;
	name = d.name;
	of = d.of;
	ofchar = d.ofchar;
	type = d.type;
	valid = d.valid;
	css = d.css;
	chunk = d.chunk;
	return *this;
}

OOReader::StyleInfo::StyleInfo()
		: name("erno"), position(0),type(obodypage),valid(false),filled(false),ofchar(DefaultCharFormats(true))
{
	QTextFormat page;
	/* default page  size if non can read */
	/* if body size page is found redraw all PageSizeID - PageMarginID
	to set % widht to elements if having and at endo to know pagecount */
	QSizeF defaultBody(CM_TO_POINT(20.999),CM_TO_POINT(29.699));
	FoRegion marinArea;
	marinArea.toAll(CM_TO_POINT(1.8));
	marinArea.padding = MM_TO_POINT(5);
	page.setProperty(PageSizeID,defaultBody);
	page.setProperty(PageMarginID,marinArea);
	of = page;
}



ChildImport::ChildImport( QIODevice* device )
		:QXmlStreamReader( device )
{
	setNamespaceProcessing(false);
	/* only prefix append if exist */
}


void ChildImport::copyDeep( QIODevice* device , QXmlStreamWriter &out  )
{
	setDevice(device);
	while (!atEnd()) {
		readNext();
		if ( isStartElement() ) {
			/* a dom element node is start to read */
			/////qDebug() << "### copydeep element tagname   ---------- " << name().toString();
			/////////qDebug() << "### copydeep element tagname   ---------- " << prefix().toString();
			if (prefix().toString().isEmpty()) {
				out.writeStartElement(name().toString());
			}
			else {
				out.writeStartElement(prefix().toString()+":"+name().toString());
			}
			if (attributes().size() > 0) {
				out.writeAttributes(attributes());
			}
		}
		else if (!isWhitespace()) {
			/* element having text or ??? */
			out.writeCharacters(text().toString());
		}
		else if (isComment()) {
			/* leave not import */
		} else if (isCDATA()) {
			/* take cdata */
			out.writeComment(text().toString());
		}


	}
	const int erno = (int)error();
	if (erno != 0) {
		//////qWarning() << "### ChildImport::copyDeep error    ---------- " << error();
	} else {
		out.writeEndElement();
	}
	device->close();
}











/* ###############################################################################################################*/











QTextBlockFormat OOReader::TextBlockFormFromDom( const QDomElement e , QTextBlockFormat pf  )
{
	QDomNamedNodeMap attlist = e.attributes();
	for (int i=0; i<attlist.count(); i++) {
		QDomNode nod = attlist.item(i);

		if (nod.nodeName().toLower() == "fo:break-before") {
			pf.setPageBreakPolicy ( QTextFormat::PageBreak_AlwaysBefore );
		}

		if (nod.nodeName().toLower() == "fo:break-after") {
			pf.setPageBreakPolicy ( QTextFormat::PageBreak_AlwaysAfter );
		}

		if (nod.nodeName().toLower() == "fo:line-height") {

		}
		if (nod.nodeName().toLower() == "fo:background-color") {
			pf.setBackground ( QBrush( FoCol->foColor(nod.nodeValue(),FopColor::Transparent) ) );
		}

		if ( nod.nodeName().toLower() == "fo:space-after" || nod.nodeName().toLower() == "fo:margin-bottom" ) {    /* bottom */
			pf.setBottomMargin(Unit(nod.nodeValue()));
		}

		if ( nod.nodeName().toLower() == "fo:space-start" || nod.nodeName().toLower() == "fo:margin-right" ) {  /* right */
			pf.setRightMargin(Unit(nod.nodeValue()));
		}

		if ( nod.nodeName().toLower() == "fo:space-end" || nod.nodeName().toLower() == "fo:margin-left" ) {   /* left */
			pf.setLeftMargin(Unit(nod.nodeValue()));
		}
		if ( nod.nodeName().toLower().contains("fo:start-indent") ) {   /* indent */
			pf.setLeftMargin(Unit(nod.nodeValue()) + pf.leftMargin() );
			pf.setProperty(QTextFormat::TextIndent,Unit(nod.nodeValue()));
		}



	}
	pf.setAlignment(  TagAlignElement(e) );
	return pf;
}

QTextCharFormat OOReader::TextCharFormFromDom( const QDomElement e , QTextCharFormat pf )
{
	charsRead++;
	bool boldText = false;
	pf.setForeground(QBrush(Qt::black));
    const QString nameFont = e.attribute ("style:font-name");
	/* fo and style mixed value on OO format */
    if (!nameFont.isEmpty()) {
    const QDomElement fontcopy = fontname[nameFont];
    Q_ASSERT(!fontcopy.isNull());
    const QString nameFont2 = fontcopy.attribute("svg:font-family");
    QFont userfont(nameFont2);
          userfont.setFamily ( nameFont2 );
                if (fontcopy.attribute("style:font-pitch") == "fixed") {
                pf.setFontFixedPitch ( true );
                } else {
                pf.setFontFixedPitch ( false );
                }     
    qDebug() << "### userfont.exactMatch()  " << userfont.exactMatch()  << nameFont2;
    pf.setFont(userfont);
    } else {
    pf.setFont(standardFont);   /* first at top on font tree */
    } 
    
   
    
	QDomNamedNodeMap attlist = e.attributes();
	for (int i=0; i<attlist.count(); i++) {
		QDomNode nod = attlist.item(i);
		const QString value = nod.nodeValue().toLower();

		if (nod.nodeName().toLower() == "fo:background-color") {
			pf.setBackground ( QBrush( FoCol->foColor(nod.nodeValue(),FopColor::Transparent) ) );
		}


		if (nod.nodeName().toLower() == "fo:baseline-shift" ||
		        nod.nodeName().toLower() == "style:text-position" ) {
			if (nod.nodeValue().toLower().startsWith("super") ||
			        nod.nodeValue().toLower().startsWith("+") ||
			        nod.nodeValue().toLower().startsWith("1")) {
				pf.setVerticalAlignment ( QTextCharFormat::AlignSuperScript );
			}
			else {
				pf.setVerticalAlignment ( QTextCharFormat::AlignSubScript );
			}
		}

		if (nod.nodeName().toLower() == "fo:color") {
			pf.setForeground(  QBrush( FoCol->foColor(nod.nodeValue(),FopColor::DarkColor) )  );
		}

		////if (nod.nodeName().toLower() == "fo:font-family" || nod.nodeName().toLower() == "style:font-name") {
			
		/////}









		if (nod.nodeName().toLower() == "fo:external-destination" || nod.nodeName().toLower() == "fo:internal-destination") {
			pf.setAnchorHref(nod.nodeValue());
			pf.setAnchor(true);
			pf.setForeground ( QBrush ( _LINK_COLOR_ ) );  /* red link */
		}


		/////if (value.contains("bold")) {
		//////
		///////}



		if (value.contains("bold")) {
			boldText = true;
		}
		if (value.contains("italic")) {
			pf.setFontItalic(true);
		}
		if (value.contains("underline")) {
			pf.setUnderlineStyle ( QTextCharFormat::SingleUnderline );
		}
		if (value.contains("oblique")) {
			pf.setFontItalic(true);
		}
		if (value.contains("overline")) {
			pf.setFontOverline(true);
		}

		if (value.contains("line-through")) {
			pf.setFontStrikeOut ( true );
		}


	}
	/* after font is set and all other */
	const qreal pointfont =  qMax( Unit(e.attribute ("fo:font-size")) , Unit(e.attribute ("font-size-complex"))  );
	if (pointfont > 2) {
		pf.setFontPointSize(pointfont);
		FontMaxPoint = qMax (FontMaxPoint,pointfont);
		FontMinPoint = qMax (FontMinPoint,pointfont);
	}
	if (e.attribute ("style:text-underline-type") == "single" ||
	        e.attribute ("style:text-underline-style") == "solid" ||
	        e.attribute ("style:text-underline-width") == "auto" ) {
		pf.setUnderlineStyle ( QTextCharFormat::SingleUnderline );
	}

	if (!e.attribute("style:text-scale").isEmpty() || !e.attribute("fo:letter-spacing").isEmpty() ) {
		QString value = qMax(e.attribute("style:text-scale"),e.attribute("fo:letter-spacing"));
		value = value.replace("%",""); /* leave % if exist */
		qreal xx = value.toDouble();
		if (xx != 0.) {
			pf.setFontLetterSpacing(xx);
		}
	}

	if (boldText) {
		pf.setFontWeight(QFont::Bold);
	}
	else {
		pf.setFontWeight(QFont::Normal);
	}

	const int debugspace = pf.fontLetterSpacing();
	if (debugspace == (int)DEBUgletterspacing )  {
		pf.setFontLetterSpacing(100);
	}


	//////////// style:text-underline-width="auto" fo:font-size="11pt" style:text-underline-style="solid"

	return pf;
}

void OOReader::TextBlockFormatPaint( const QString name , const QDomElement e )
{
	if (name.size() < 1) {
		return;
	}
	QTextFormat bblock = TextBlockFormFromDom(e,DefaultMargin());
	QDomElement es = e.nextSiblingElement ("style:text-properties");
	QDomElement ei = e.nextSiblingElement ("style:background-image");

	if (css2[name].valid) {
		css2[name].of = bblock;
		if (!es.isNull()) {
			css2[name].ofchar = TextCharFormFromDom(es,DefaultCharFormats(QTWRITTELN));
		}
		css2[name].filled = true;
#ifdef _OOREADRELEASE_
		css2[name].css = cssGroup(e);
		css2[name].ofchar.setToolTip ( name );
#endif
	}
	///////////qDebug() << "### TextBlockFormatPaint  " << e.tagName() << name;
}

void OOReader::TextCharFormatPaint( const QString name , const QDomElement e )
{
	//////qDebug() << "### spin a " << name;

	if (name.size() < 1) {
		return;
	}
	QTextCharFormat  Charformat = TextCharFormFromDom(e,DefaultCharFormats(QTWRITTELN));
	if (css2[name].valid) {
		css2[name].filled = true;
#ifdef _OOREADRELEASE_
		Charformat.setToolTip ( name );
		css2[name].css = cssGroup(e);
#endif
		css2[name].ofchar = Charformat;
	}
	//////qDebug() << "### TextCharFormatPaint  " << e.tagName() << name;

}

void OOReader::FrameImageFormatPaint( const QString name , const QDomElement e )
{
	const QDomElement im = e.firstChildElement("style:graphic-properties");
	if (!im.isNull()) {
    QColor bg = FoCol->foColor(im.attribute("fo:background-color"),FopColor::Transparent);
    int goalpha = OoColorAlpha(QString(im.attribute("style:background-transparency")).replace("%","").toInt());
    bg.setAlpha(goalpha);  
    QTextFrameFormat fox;
    QString sborder = im.attribute("fo:border");
    QStringList tris = sborder.split(" ");
	fox.setPadding ( qMax(Unit(e.attribute("fo:padding")), Unit("2mm") ) );
    if (tris.size() == 3) {
	fox.setBorder ( Unit(tris.at(0)) );
	fox.setBorderBrush ( FoCol->foColor(tris.at(2),FopColor::DarkColor) );
	fox.setBorderStyle ( QTextFrameFormat::BorderStyle_Solid );
    }   
	fox.setBackground ( bg );
		/* copy dom to having copy down */
		if (css2[name].valid) {
            css2[name].of = fox;
			css2[name].chunk = CatDomElement(e);
			css2[name].filled = true;
#ifdef _OOREADRELEASE_
			css2[name].css = cssGroup(e);
#endif
		}
	}
}

void OOReader::TextListFormatPaint( const QString name , const QDomElement e )
{
	//////qDebug() << "### spin a " << name;
	if (e.tagName() != QLatin1String( "text:list-style" )) {
		return;
	}
	//////////////  <text:list-level-style-bullet text:bullet-char="?" text:level="1" >
	const QDomElement nx = e.firstChildElement("text:list-level-style-number");
	const QDomElement nd = e.firstChildElement("text:list-level-style-bullet");

	if (name.size() < 1) {
		return;
	}
	QTextListFormat  listformat;
	QTextListFormat::Style style;

	const uint CharBull = nd.attribute("text:bullet-char","z").at(0).unicode();

	if ( nx.attribute ("style:num-format") == QString::fromLatin1("1") && nd.isNull()) {
		style = QTextListFormat::ListDecimal;
	}
	else if ( nx.attribute ("style:num-format") == QString::fromLatin1("a") && nd.isNull()  ) {
		style = QTextListFormat::ListLowerAlpha;
	}
	else if ( nx.attribute ("style:num-format") == QString::fromLatin1("A") && nd.isNull() ) {
		style = QTextListFormat::ListUpperAlpha;
	}
	else if (  CharBull == 9675  ) {
		style = QTextListFormat::ListCircle;
	}
	else if (  CharBull == 9679  ) {
		style = QTextListFormat::ListDisc;
	}
	else {
		style = QTextListFormat::ListSquare;
	}
	listformat.setStyle ( style );
	/* indent from tree auto */
	if (css2[name].valid) {
		//////qDebug() << "### spin b ";
		css2[name].of = listformat;
		css2[name].filled = true;
#ifdef _OOREADRELEASE_
		css2[name].css = cssGroup(e);
#endif
	}

	///////////qDebug() << "### CharBull set   " << CharBull  << " - " << nd.attribute("text:bullet-char","z");

}

void OOReader::UnknowFormatPaint( const QString name , const QString style_name  ,  const QDomElement e )
{
#ifdef _OOREADRELEASE_
	if (css2[name].valid) {
		css2[name].css = cssGroup(e);
	}
#endif
	QTextFrameFormat base;
	QTextTableFormat table;
	QTextTableCellFormat cell;
	if ( style_name == QLatin1String( "table-column" ) ) {
		/* save column */
		const qreal width = Unit(e.firstChildElement("style:table-column-properties").attribute("style:column-width"));
		qDebug() << "### style:table-column-properties  widthwidthwidthwidthwidthwidth  " << width;



		if (css2[name].valid) {
			if (width > 0) {
				base.setWidth ( width );
				css2[name].of = base;
			}
#ifdef _OOREADRELEASE_
			css2[name].chunk = CatDomElement(e);
			css2[name].css = cssGroup(e);
#endif
			css2[name].filled = true;
		}
		else {
			///////////qWarning() << "Not register style name ->" << style_name << name;
		}

	} else if ( style_name == QLatin1String( "table" ) ) {
		////////  style:table-properties
		const QDomElement tablecss = e.firstChildElement("style:table-properties");
		const qreal width = Unit(tablecss.attribute("style:width"));
		///////////////qDebug() << "### table width  ->" << width  <<  " real" << tablecss.attribute("style:width");

		if (css2[name].valid) {
			table.setWidth ( width );  /* checks on set */
			table.setAlignment(TagAlignElement(tablecss));
			table.setCellPadding(0);  /* oo padding on cell grep on loop max padding */
			table.setCellSpacing(0);
			table.setBorder ( 0.2 );
			table.setBackground ( QBrush( FoCol->foColor(tablecss.attribute("fo:background-color"),FopColor::Transparent) ) );
			css2[name].of = table;
#ifdef _OOREADRELEASE_
			css2[name].chunk = CatDomElement(e);
			css2[name].css = cssGroup(e);
#endif
			css2[name].filled = true;
		}
		else {
			////////////qWarning() << "Not register style name ->" << style_name << name;
		}
	}  else if ( style_name == QLatin1String( "table-cell" ) ) {
		QDomElement celcss = e.firstChildElement("style:table-cell-properties");

		cell.setBottomPadding ( qMax(Unit(celcss.attribute("fo:padding")) , Unit(celcss.attribute("fo:padding-bottom")))  );
		cell.setRightPadding ( qMax(Unit(celcss.attribute("fo:padding")) , Unit(celcss.attribute("fo:padding-right")))  );
		cell.setTopPadding( qMax(Unit(celcss.attribute("fo:padding")) , Unit(celcss.attribute("fo:padding-top")))  );
		cell.setLeftPadding ( qMax(Unit(celcss.attribute("fo:padding")) , Unit(celcss.attribute("fo:padding-left")))  );

		cell.setBackground ( QBrush( FoCol->foColor(celcss.attribute("fo:background-color"),FopColor::Transparent) ) );
		cell.setProperty(CellBorderStyleID,celcss.attribute("fo:border"));

		//////qDebug() << "### table-cell color st size ->" << celcss.attribute("fo:background-color").size();

		if (css2[name].valid) {
			css2[name].of = cell;
#ifdef _OOREADRELEASE_
			css2[name].chunk = CatDomElement(e);
			css2[name].css = cssGroup(e);
#endif
			css2[name].filled = true;
		}
		else {
			////////////qWarning() << "Not register style name ->" << style_name << name;
		}
	}


}


void OOReader::registerFontDoc( const QDomElement &element )
{
    if (element.isNull()) {
    return;
    }
    qDebug() << "### registerFontDoc  standardFont.name  -> " << standardFont.defaultFamily();
    qDebug() << "### registerFontDoc  standardFont.name  -> " << standardFont.family();
    qDebug() << "### registerFontDoc  standardFont.name  -> " << standardFont.toString();
     
    
    
    QDomNode child = element.firstChild();
	while ( !child.isNull() ) {
		if ( child.isElement() ) {
            const QDomElement el = child.toElement();
            if ( el.tagName() == QLatin1String( "style:font-face" ) ) {
                const QString fonts = el.attribute ("style:name");
                if (!fonts.isEmpty ()) {
                    fontTotal++;
                    if (fontTotal == 1) {
                     standardFont = QFont(el.attribute ("svg:font-family"),10);
                    }
                    fontname.insert(fonts,el);
                }
                registerFontDoc(el.firstChildElement() );                
            }
        }
        child = child.nextSibling();
    }
}


void OOReader::convertStyleNameRoot( const QDomElement &element )
{
    if (element.isNull()) {
    return;
    }
    
	QDomNode child = element.firstChild();
	while ( !child.isNull() ) {
		if ( child.isElement() ) {
			const QDomElement el = child.toElement();
            
			
            
			if ( el.tagName() == QLatin1String( "style:style" ) ) {
				const QString sname = el.attribute ("style:name");
                
				///// paragraph or text
				const QString forType = el.attribute ("style:family");
                
                qDebug() << "### forType ......" << forType;
                
                
				if (forType == QLatin1String("text") ) {
					TextCharFormatPaint(sname,el.firstChildElement("style:text-properties"));
				}
				else if (forType == QLatin1String("paragraph")) {
					TextBlockFormatPaint(sname,el.firstChildElement("style:paragraph-properties"));
				}
				else if (forType == QLatin1String("graphic")) {
					FrameImageFormatPaint(sname,el);
				}
				else {
					UnknowFormatPaint(sname,forType,el);
				}
			}
			else if ( el.tagName() == QLatin1String( "text:list-style" ) ) {
				const QString sname = el.attribute ("style:name");
				TextListFormatPaint(sname,el);
			}
			else {


			}
		}
		child = child.nextSibling();
	}
}


/* table column  format + frame */
QTextFrameFormat OOReader::FrameFormat( const QString name )
{
	QTextFrameFormat init;
	init.setWidth ( -1 );
	QTextFrameFormat base;
	if (css2[name].valid) {
		return css2[name].of.toFrameFormat();
	}
	else {
		return init;
	}
}



bool OOReader::convertCellTable( const QDomElement e  , QTextCursor &cur  , const int processing )
{

	cur.beginEditBlock();
	///////cur.insertText(QString("%1-%2").arg(cell.row()).arg(cell.column()));
	/////cur.insertText(QString("cc"));
	/////cur.endEditBlock();
	/////////return true;


	QDomElement child = e.firstChildElement();
	int posi = -1;
	while ( !child.isNull() ) {
		if ( child.tagName() == QLatin1String( "text:p" ) ) {
			posi++;
			if (posi == 0) {
				cur.beginEditBlock();
			}
			else {
				cur.insertBlock();
				cur.beginEditBlock();
			}
			convertBlock(cur,child,0);
			styleCurrentCount++;
			emit statusRead(styleCurrentCount,sumOfBlocks);
		}
		else if ( child.tagName() == QLatin1String( "text:list" ) ) {
			convertList(cur,child,styleCurrentCount);
			styleCurrentCount++;
			emit statusRead(styleCurrentCount,sumOfBlocks);
		}
		else if ( child.tagName() == QLatin1String( "table:table" ) ) {
			convertTable(cur,child,styleCurrentCount);
		}
		else if (child.isText()) {
			cur.insertText(child.nodeValue()+"|TEXT_NODE_ROOT_ON_CELL|");
		}
		child = child.nextSiblingElement();
	}

	cur.endEditBlock();
	return true;
}




bool OOReader::convertTable( QTextCursor &cur , const QDomElement e  , const int processing , bool roottable )
{
	/* 1441 from fop convert */
	///////qDebug() << "### convertTable.---" << styleCurrentCount << sumOfBlocks << " tag " << e.tagName();
	//////return true;
	bool  UseTdIter  = false;
	QMap<int,QDomElement> allcell;

	const QString tname = e.attribute ("table:style-name","default");
	/* column count and sett wi distance */
	QVector<QTextLength> constraints;
	int colls = 0;
	QDomElement column = e.firstChildElement("table:table-column");
	const qreal defaultPercents = 99.9999;
	while (!column.isNull()) {
		QTextLength cool_wi = QTextLength(QTextLength::PercentageLength,defaultPercents);
		bool appendC = false;
		const QString sname = column.attribute ("table:style-name");
		const int makecolls = qMax(column.attribute("table:number-columns-repeated").toInt(),1);
		if (!sname.isEmpty()) {
			/* QTextFrameFormat */
			cool_wi = FrameFormat(sname).width();
			if (cool_wi.rawValue() < 1) {
				cool_wi = QTextLength(QTextLength::PercentageLength,defaultPercents);
			}
		}
		for (int i = 0; i < makecolls; ++i) {
			colls++;
			constraints.insert(constraints.size(),cool_wi);
		}
		column = column.nextSiblingElement("table:table-column");
	}
	const qreal coolsPercents = 100 / colls;
	for (int i = 0; i < constraints.size(); ++i) {
		QTextLength modcool = constraints.at ( i );
		if (modcool.rawValue() == defaultPercents) {
			constraints.replace(i,QTextLength(QTextLength::PercentageLength,coolsPercents));
		}
	}

	if (colls < 1) {
		return false;
	}

	cur.insertBlock();
	cur.beginEditBlock();
	int rowCounter = 0;
	Q_ASSERT(colls ==  constraints.size());
	QDomElement trow = e.firstChildElement("table:table-row");
	QDomNode line = e.firstChild();
	while (!trow.isNull()) {
		rowCounter++;
		trow = trow.nextSiblingElement("table:table-row");
	}
	QTextTableFormat tf;
	if (css2[tname].valid) {
		tf = css2[tname].of.toTableFormat();
	}
	else {
		tf.setWidth(QTextLength(QTextLength::PercentageLength,100));
	}
	tf.setColumnWidthConstraints(constraints);

	const int Columns = constraints.size();
	const int Rows = rowCounter;

	QTextTable *qtable = cur.insertTable(Rows,Columns,tf);
	int gorow = -1;
	int gocool = -1;
	allcell.clear();
	while ( !line.isNull() ) {
		if (line.isElement() ) {
			const QDomElement oorow = line.toElement();
			if (oorow.tagName() == "table:table-row") {
				gorow++;
				////////////////qDebug() << "### row ------------------------------------------- " << gorow;
				gocool = -1;
				QTextTableCell cell;
				QDomElement oocell = oorow.firstChildElement();
				while (!oocell.isNull())  {
					bool increment = false;
					/*
					if (gocool >= 0) {
					     check row before if spanrows?
					    const QTextTableCell checkCell = qtable->cellAt(gorow - 1,gocool);
					    if (checkCell.isValid()) {
					     if (checkCell.rowSpan() > 1) {
					     increment = true;
					     }
					    }
					}
					 */
					if ( oocell.tagName() == QLatin1String( "table:covered-table-cell" ) || increment ) {
						gocool++;
						/* is filled on before row -> row span table:number-rows-spanned !!! */
					}
					else if ( oocell.tagName() == QLatin1String( "table:table-cell" ) ) {
						gocool++;
						/* normal td cell */
						const int cr = qMax(oocell.attribute("table:number-rows-spanned").trimmed().toInt(),0);
						const int cs = qMax(oocell.attribute("table:number-columns-spanned").trimmed().toInt(),0);
						if (cs > 0) {
							qtable->mergeCells (gorow,gocool,1,cs);
							cell = qtable->cellAt(gorow,gocool);
						}
						else  if (cr > 0) {
							qtable->mergeCells (gorow,gocool,cr,1);
							cell = qtable->cellAt(gorow,gocool);
						}
						else {
							cell = qtable->cellAt(gorow,gocool);
						}
						if (cell.isValid()) {

							QTextCharFormat tdformat = cell.format();
							QTextCursor tmpcur  = cell.firstCursorPosition();
							const QString name = oocell.attribute ("table:style-name","default");
							if (css2[name].valid) {
								QTextTableCellFormat cefo = css2[name].of.toTableCellFormat();
								tdformat.merge(cefo);
								cell.setFormat ( tdformat );
							}

							if (UseTdIter) {
								allcell.insert(allcell.size(),oocell);
							}
							else {
								QTextCursor tmpcur  = cell.firstCursorPosition();
								convertCellTable(oocell,tmpcur,processing);
							}


						}
						else {
							qWarning() << "!!!! not valid cell at->  row:" << gorow << " column:" << gocool;
						}
					}
					oocell = oocell.nextSiblingElement();
				}
			}
		}
		line = line.nextSibling();
	}
	if (UseTdIter) {
		int celliter = -1;
		QTextFrame::Iterator frameIt = qtable->begin();
		for (QTextFrame::Iterator it = frameIt;  !it.atEnd(); ++it) {
			celliter++;
			if (it.currentBlock().isValid()) {
				const QDomElement tdcell = allcell[celliter];
				if (!tdcell.isNull()) {
					QTextCursor cursub(Qdoc);
					cursub.setPosition(it.currentBlock().position());
					convertCellTable(tdcell,cursub,processing);
				}
				else {
					qWarning() << "!!!! dom cell not valid " << celliter;
				}
			}

		}

	}

	//////QTextFrameFormat fox = DefaultFrameFormat();
	///////QTextFrame *foxtable = cur.currentFrame();
	//////qtable->setFrameFormat ( fox );
	cur.endEditBlock();

	if (roottable) {
		cur.movePosition(QTextCursor::End);
	}
	return true;
}















