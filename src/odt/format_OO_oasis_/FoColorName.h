#ifndef FOPCOLORFORMAT_H
#define FOPCOLORFORMAT_H


#include <QtCore>
#include <QDebug>
#include <QCoreApplication>
#include <QColor>
#include <QMap>
#include <QPixmap>
#include <QDomDocument>
#include <QTextTableFormat>
#include <QTextFrameFormat>
#include <QTextFrame>
#include <QDomDocument>
#include <QTextTableFormat>
#include <QTextDocument>

#define POINT_TO_CM(cm) ((cm)/28.3465058)
#define POINT_TO_MM(mm) ((mm)/2.83465058)     ////////  0.352777778
#define POINT_TO_DM(dm) ((dm)/283.465058)
#define POINT_TO_INCH(inch) ((inch)/72.0)
#define POINT_TO_PI(pi) ((pi)/12)
#define POINT_TO_DD(dd) ((dd)/154.08124)
#define POINT_TO_CC(cc) ((cc)/12.840103)

#define MM_TO_POINT(mm) ((mm)*2.83465058)
#define CM_TO_POINT(cm) ((cm)*28.3465058)     ///// 28.346456693
#define DM_TO_POINT(dm) ((dm)*283.465058)
#define INCH_TO_POINT(inch) ((inch)*72.0)
#define PI_TO_POINT(pi) ((pi)*12)
#define DD_TO_POINT(dd) ((dd)*154.08124)
#define CC_TO_POINT(cc) ((cc)*12.840103)



/*  QColor  alpha color 0 - 100%  100% = full transparent */
#define ALPHACOLPER(aa) ((aa)*2.555555)
static inline qreal OoColorAlpha( const int i )
{
    if (i > 0 && i < 101) {
    return 255 - ALPHACOLPER(i);
    } else {
    return 255;
    }
}


#define _USELISTAPACHECOLOR_  1

/*

    QTextEdit t;
    FopColor *fc = new FopColor();
    qDebug() << "### load color   ";
    QStringList items = fc->fopListColor();
    for (int i = 0; i < items.size(); ++i)  {
        const QString name = items.at(i);
        QTextCursor c = t.textCursor();
        QPixmap e =  fc->createColorMap(name);
        t.document()->addResource(QTextDocument::ImageResource,QUrl(name),e);
        QTextImageFormat format;
        format.setName( name );
        format.setHeight ( e.height() );
        format.setWidth ( e.width() );
        format.setToolTip(name);
        t.textCursor().insertImage( format );
    }

    t.show();
*/




/*  grab color from fop format ;
    alpha color only on rgb(11,11,11,110)  last


*/
/* fix from Peter Hohl to make image running on dpi unit! */
static inline qreal DPIactualDiff()
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    int hi = qMax(pixmap.logicalDpiY(),72);
    int mi = qMin(pixmap.logicalDpiY(),72);
    if (hi == mi) {
    return 1; /* no difference increase or degrease 1:1 running 72 DPI  */
    }
    const qreal precision = 0.09741;   /* gimp precision to image */
    ////////return 1;   /* 1:1 to test */
    int diff = hi - mi;
    qreal increments = 1.;
    if (mi == 72) {  //////  more >>>
    return increments + (diff / 100.0) + precision;
    } else {   ///////////// down <<<
    return increments - (diff / 100.0) + precision;
    }
}

#define PDIFIX(ii) ((ii)*DPIactualDiff())


QString metrics( const qreal screenpoint );
/* metric conversion from and to */
qreal Unit( const QString datain );
qreal Pointo( qreal unit , const QString unita );
qreal ToUnit( qreal unit , const QString unita );
qreal ToPoint( qreal unit , const QString unita );



class FopColor
{

public:
    
typedef enum
{  
  DarkColor = 100,
  LightColor = 200,
  Transparent = 300
} AlternateColor;
///static 
FopColor( const QString colorchunk = QString() , FopColor::AlternateColor col = Transparent );
QColor foColor( const QString colorchunk , FopColor::AlternateColor col = DarkColor );
QString foName( const QColor e );
QPixmap createColorMap( const QString colorchunk );
/////  static to retun name human readable ? 
inline QStringList fopListColor() { return fopcolorNames; }
inline QColor color() { return currentcolor; }
inline int size() { return fopcolorNames.count(); }
protected:
   QMap<QString,QColor> foplist;   /* forever clear only construct */
   QMap<QString,QColor> avaiablelist; 
   QStringList fopcolorNames;
private:
    QColor alternateColor( FopColor::AlternateColor col );
   QColor currentcolor;
   void record( const QString colorchunk , QColor item );

};


class FoRegion
{
public:
  FoRegion();
  FoRegion& operator=( const FoRegion& d );
  operator QVariant() const
  {
    return QVariant::fromValue(*this);
  }
  QString hash() const;
  void styleContainer( QDomElement e , const qreal height = 0 );
  void styleSmallDocument( QTextDocument *doc );
  void styleReadDocument( const QTextDocument *doc );
  void fomax(  FoRegion& m ,  FoRegion& d );
  /* same margin to all */
  void toAll( const qreal unique );
  void marginNoBody();
  QMap<QString,QPen> penstyle();  /* return avaiable pen */
  QColor bg; /* background color */
  QColor bog; /* border color */
  qreal margin_top;
  qreal margin_bottom;
  qreal margin_left;
  qreal margin_right;
  qreal border;
  qreal padding;
  QPen rpen;
  bool enable;
  QByteArray edom;   /* dome element*/
};

Q_DECLARE_METATYPE(FoRegion)





#endif







