#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <QWidget>

namespace Ui {
class Convertor;
}

class Convertor : public QWidget
{
    Q_OBJECT

public:
    explicit Convertor(QWidget *parent = 0);
    ~Convertor();

private:
    Ui::Convertor *ui;
};

#endif // CONVERTOR_H
