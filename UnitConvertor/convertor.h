#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <QWidget>
#include <QPair>

#include "unit.h"
#include "unitmodel.h"
#include "convertoroperator.h"
namespace Ui {
class Convertor;
}
namespace GMTOOL
{


class Convertor : public QWidget
{
    Q_OBJECT

public:
    explicit Convertor(QWidget *parent = 0);
    virtual ~Convertor();

public slots:
    void categoryHasChanged(int i);
    void convert();
private:
    Ui::Convertor *ui;
    QMap<Unit::Category,QString> m_map;
    UnitModel* m_model;
    CategoryModel* m_catModel;
    CategoryModel* m_toModel;

    QHash<QPair<Unit*,Unit*>,ConvertorOperator*> m_convertorTable;
};
}
#endif // CONVERTOR_H
