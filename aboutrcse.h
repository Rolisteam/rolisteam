#ifndef ABOUTRCSE_H
#define ABOUTRCSE_H

#include <QDialog>

namespace Ui {
class AboutRcse;
}

class AboutRcse : public QDialog
{
    Q_OBJECT

public:
    explicit AboutRcse(QWidget *parent = 0);
    ~AboutRcse();

private:
    Ui::AboutRcse *ui;
};

#endif // ABOUTRCSE_H
