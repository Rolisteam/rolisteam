#include "OOFormat.h"



static QString txtcapitalize( QString & s)
{
    return (s.isEmpty()) ? s : s.left(1).toUpper() + s.mid(1).toLower();
}







QByteArray CatDomElement( const QDomElement e )
{
    if (e.isNull()) {
     return QByteArray(); 
    }
    QDomDocument doc;
    QDomElement Pbb = doc.createElement(e.tagName());
    QDomNamedNodeMap alist = e.attributes();  
    for (int i=0; i<alist.count(); i++){
    QDomNode nod = alist.item(i);
    Pbb.setAttribute(nod.nodeName().toLower(),nod.nodeValue());
    }  
    doc.appendChild(Pbb);
            
               QDomNode child = e.firstChild();
					while ( !child.isNull() ) {
						if ( child.isElement() ) {
					    Pbb.appendChild(doc.importNode(child,true).toElement());
					    }
				     child = child.nextSibling();            
				   }
            
    return doc.toByteArray(1);
}




#ifdef _OOREADRELEASE_
/* debug all item incomming in tree 2 level */
QString cssGroup( const QDomElement e )
{
    QStringList cssitem;
    QDomNamedNodeMap alist = e.attributes();  
       for (int i=0; i<alist.count(); i++){
            QDomNode nod = alist.item(i);
            cssitem.append(QString("%1:%2").arg(nod.nodeName().toLower()).arg(nod.nodeValue()));
       }  
                    QDomNode child = e.firstChild();
					while ( !child.isNull() ) {
						if ( child.isElement() ) {
					        QDomNamedNodeMap list = child.attributes(); 
                             for (int x=0; x<list.count(); x++){
                               QDomNode nod = list.item(x);  
                               cssitem.append(QString("%1:%2").arg(nod.nodeName().toLower()).arg(nod.nodeValue())); 
                             }
					    }
				     child = child.nextSibling();            
				   }
       
    return cssitem.join(";");
}
#endif

QTextFrameFormat DefaultFrameFormat()
{
    QTextFrameFormat base;
    base.setBorderBrush ( Qt::blue );
    base.setBorderStyle ( QTextFrameFormat::BorderStyle_Solid );
    base.setBorder ( 0.6 );
    return base;
}

QTextCharFormat DefaultCharFormats( bool qtwritteln , QTextCharFormat format )
{
    #if QT_VERSION >= 0x040500
    format.setFontStyleStrategy ( QFont::PreferAntialias );
    #endif
    return format;
}

/* set all margin to zero qt4 send 12 top 12 bottom by default */
QTextBlockFormat DefaultMargin( QTextBlockFormat rootformats )
{
    rootformats.setBottomMargin(4);
    rootformats.setTopMargin(4);
    rootformats.setRightMargin(0);
    rootformats.setLeftMargin(0);
    rootformats.setAlignment( Qt::AlignLeft );
    return rootformats;
}

QTextCharFormat PreFormatChar( QTextCharFormat format )
{
    format.setFont(QFont( "courier",11,-1,true) );
    format.setForeground(QBrush(Qt::darkBlue));
    format.setFontLetterSpacing(90);
    ///////format.setBackground(QBrush(Qt::lightGray));
    return format;
}



QString  ootrimmed( QString txt , const QString txttransform )
{


    txt.replace("&amp;", "&");   /*  qt4 toUtf8 dont replace && */
    txt.replace("&lt;", "<");
    txt.replace("&gt;", ">");

    if (txttransform == QString("uppercase"))
    {
        txt = txt.toUpper();
    }
    else if (txttransform == QString("capitalize"))
    {
        txt = txtcapitalize(txt);
    }
    else if (txttransform == QString("lowercase"))
    {
        txt = txt.toLower();
    }

    const int cosize = txt.size() - 1;

    bool lastspace = false;
    bool firstspace = false;
    if (txt.size() >= 1 )
    {
        if (txt.at(0) == QChar::Null || txt.at(0) == QChar(' ') | txt.at(0) ==  QChar::Nbsp )
        {
            firstspace = true;
        }
        if (txt.at(cosize) == QChar::Null || txt.at(cosize) == QChar(' ') | txt.at(cosize) ==  QChar::Nbsp )
        {
            lastspace  = true;
        }



        QString nwex = txt.simplified();
        if (firstspace)
        {
            nwex.prepend(QChar::Nbsp);  /////// QChar::Nbsp
        }
        if (lastspace)
        {
            nwex.append(QChar::Nbsp);  /////// QChar::Nbsp
        }

        return nwex;
    }
    else
    {
        return txt;
    }
}









OOFormat::OOFormat()
 : QObject(0),Qdoc(new QTextDocument()),FoCol(new FopColor())
{
	
}



QString OOFormat::hashGenerator( const QString name  ) const
{
    QCryptographicHash enmd5( QCryptographicHash::Sha1 );
    enmd5.addData ( name.toUtf8() );
    enmd5.addData ( "odt_name" );
    const QByteArray chunkha = enmd5.result();
    return QString(chunkha.toHex());
}





