#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <QWidget>
#include "unit.h"
#include "unitmodel.h"

namespace Ui {
class Convertor;
}

class Convertor : public QWidget
{
    Q_OBJECT

public:
    explicit Convertor(QWidget *parent = 0);
    virtual ~Convertor();

public slots:
    void categoryHasChanged(int i);
private:
    Ui::Convertor *ui;
    QMap<Unit::Category,QString> m_map;
    UnitModel* m_model;
};
#endif // CONVERTOR_H
