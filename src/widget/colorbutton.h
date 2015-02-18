#ifndef COLORLABEL_H
#define COLORLABEL_H
#include <QColor>
#include <QPushButton>
/**
  * @brief shows selected color and detect any event such as click or doubleclick
  */
class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    /**
      * @brief default constructor
      */
    ColorButton(QWidget * parent = 0);
    /**
      * @brief defines the color of the colorbutton
      */
   void setColor(QColor color);
   /**
     * @brief accessor to the color.
     */
   const QColor& color() const;
signals:
   /**
     * @brief send off the current color
     */
    void colorChanged(const QColor& p);

private slots:
    /**
      * @brief is called when color has changed.
      */
    void changeColor();


private:
    QColor m_color;
};

#endif // COLORLABEL_H
