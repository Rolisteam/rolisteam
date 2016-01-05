#ifndef FIELD_H
#define FIELD_H

#include <QWidget>
#include <QLabel>

class Field : public QWidget
{
    Q_OBJECT
public:
    enum BorderLine {UP=1,LEFT=2,DOWN=4,RIGHT=8,ALL=15,NONE=16};
    explicit Field(QWidget *parent = 0);

    void writeQML();

    void drawField();




    QString key() const;
    void setKey(const QString &key);

    QPoint position() const;
    void setPosition(const QPoint &pos);

    QSize size() const;
    void setSize(const QSize &size);

    Field::BorderLine border() const;
    void setBorder(const Field::BorderLine &border);

    QColor bgColor() const;
    void setBgColor(const QColor &bgColor);

    QColor textColor() const;
    void setTextColor(const QColor &textColor);

    QFont font() const;
    void setFont(const QFont &font);

signals:
    void clickOn(Field*);

public slots:

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent* ev);
private:
    QString m_key;
    QPoint m_pos;
    QSize  m_size;
    BorderLine m_border;
    QColor m_bgColor;
    QColor m_textColor;
    QFont  m_font;


    //internal data
    QLabel* m_label;
};

#endif // FIELD_H
