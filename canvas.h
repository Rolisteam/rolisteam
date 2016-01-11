#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);

    QImage image() const;
    void setImage(const QImage &image);

signals:

public slots:

private:
    QImage m_image;
};

#endif // CANVAS_H
