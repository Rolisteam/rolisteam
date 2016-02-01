#include "field.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonArray>
#include <QUuid>

int Field::m_count = 0;
Field::Field(QGraphicsItem* parent)
{
 init();
}

Field::Field(QPointF topleft,QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    m_rect.setTopLeft(topleft);
    m_rect.setBottomRight(topleft);

    m_bgColor = QColor(Qt::white);
    m_textColor = QColor(Qt::black);
    m_font = font();
    m_key = QStringLiteral("key %1").arg(m_count);

    init();

}
void Field::init()
{
    ++m_count;
    m_id = QStringLiteral("id%1").arg(m_count);
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);


    connect(this,&Field::xChanged,[=](){
        emit updateNeeded(this);
    });
    connect(this,&Field::yChanged,[=](){
        emit updateNeeded(this);
    });
}

QRectF Field::boundingRect() const
{
    return m_rect;
}

QVariant Field::getValue(Item::ColumnId id) const
{
    switch(id)
    {
    case NAME:
        return m_key;
    case X:
        return m_rect.x();
    case Y:
        return m_rect.y();
    case WIDTH:
        return m_rect.width();
    case HEIGHT:
        return m_rect.height();
    case BORDER:
        return (int)m_border;
    case TEXT_ALIGN:
        return m_textAlign;
    case BGCOLOR:
        return m_bgColor.name(QColor::HexArgb);
    case TEXTCOLOR:
        return m_textColor.name(QColor::HexArgb);
    }
    return QVariant();
}

void Field::setValue(Item::ColumnId id, QVariant var)
{
    switch(id)
    {
    case NAME:
         m_key = var.toString();
        break;
    case X:
         m_rect.setX(var.toReal());
        break;
    case Y:
        m_rect.setY(var.toReal());
        break;
    case WIDTH:
        m_rect.setWidth(var.toReal());
        break;
    case HEIGHT:
        m_rect.setHeight(var.toReal());
        break;
    case BORDER:
        m_border = (BorderLine)var.toInt();
        break;
    case TEXT_ALIGN:
        m_textAlign= (TextAlign)var.toInt();
        break;
    case BGCOLOR:
        m_bgColor= var.value<QColor>();
        break;
    case TEXTCOLOR:
        m_textColor= var.value<QColor>();
        break;
    }
    update();
}
void Field::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();


    painter->drawRect(m_rect);
    painter->drawText(m_rect,m_key);

    painter->restore();


}
void Field::setNewEnd(QPointF nend)
{
    m_rect.setBottomRight(nend);
}

void Field::drawField()
{

}
QString Field::key() const
{
    return m_key;
}

void Field::setKey(const QString &key)
{
    m_key = key;
    drawField();
}

Field::BorderLine Field::border() const
{
    return m_border;
}

void Field::setBorder(const Field::BorderLine &border)
{
    m_border = border;
    drawField();
}
QColor Field::bgColor() const
{
    return m_bgColor;
}

void Field::setBgColor(const QColor &bgColor)
{
    m_bgColor = bgColor;
    drawField();
}
QColor Field::textColor() const
{
    return m_textColor;
}

void Field::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
    drawField();
}
QFont Field::font() const
{
    return m_font;
}

void Field::setFont(const QFont &font)
{
    m_font = font;
    drawField();
}

void Field::mousePressEvent(QMouseEvent* ev)
{
    if(ev->button() == Qt::LeftButton)
    {
       // emit clickOn(this);
    }
}

void Field::save(QJsonObject& json,bool exp)
{
    if(exp)
    {
        json["type"]="field";
        json["key"]=m_key;
        return;
    }
    json["type"]="field";
    json["key"]=m_key;
    json["border"]=m_border;

    QJsonObject bgcolor;
    bgcolor["r"]=QJsonValue(m_bgColor.red());
    bgcolor["g"]=m_bgColor.green();
    bgcolor["b"]=m_bgColor.blue();
    bgcolor["a"]=m_bgColor.alpha();
    json["bgcolor"]=bgcolor;

    QJsonObject textcolor;
    textcolor["r"]=m_textColor.red();
    textcolor["g"]=m_textColor.green();
    textcolor["b"]=m_textColor.blue();
    textcolor["a"]=m_textColor.alpha();
    json["textcolor"]=textcolor;

    json["font"]=m_font.toString();
    json["textalign"]=m_textAlign;
    json["x"]=m_rect.x();
    json["y"]=m_rect.y();
    json["width"]=m_rect.width();
    json["height"]=m_rect.height();
}

void Field::load(QJsonObject &json,QGraphicsScene* scene)
{
    m_key = json["key"].toString();
    m_border = (BorderLine)json["border"].toInt();


    QJsonObject bgcolor = json["bgcolor"].toObject();
    int r,g,b,a;
    r = bgcolor["r"].toInt();
    g = bgcolor["g"].toInt();
    b = bgcolor["b"].toInt();
    a = bgcolor["a"].toInt();

    m_bgColor=QColor(r,g,b,a);

    QJsonObject textcolor = json["textcolor"].toObject();

    r = textcolor["r"].toInt();
    g = textcolor["g"].toInt();
    b = textcolor["b"].toInt();
    m_textColor=QColor(r,g,b,a);

    m_font.fromString(json["font"].toString());

    m_textAlign = (TextAlign)json["textalign"].toInt();
    qreal x,y,w,h;
    x=json["x"].toDouble();
    y=json["y"].toDouble();
    w=json["width"].toDouble();
    h=json["height"].toDouble();

    m_rect.setRect(x,y,w,h);


    update();
}

void Field::generateQML(QTextStream &out,Item::QMLSection sec)
{
    if(sec==Item::FieldSec)
    {
    out << "Field {\n";
    out << "    id:"<<m_id<< "\n";
    out << "    text: _model.getValue(\""<<m_key << "\")\n";
    out << "    x:" << m_rect.x() << "*parent.realScale"<<"\n";
    out << "    y:" << m_rect.y()<< "*parent.realScale"<<"\n";
    out << "    width:" << m_rect.width() <<"*parent.realScale"<<"\n";
    out << "    height:"<< m_rect.height()<<"*parent.realScale"<<"\n";
    out << "    color: \"" << m_bgColor.name(QColor::HexArgb)<<"\"\n";
    out << "}\n";
    }
    else if(sec==Item::ConnectionSec)
    {
        out << "if(valueKey==\""<<m_key<<"\")"<<"\n";
        out << "           {"<<"\n";
        out << "                "<<m_id.toLower().trimmed()<<".text=value;"<<"\n";
        out << "            }"<<"\n";
    }
}







