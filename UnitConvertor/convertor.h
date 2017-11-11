#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <QWidget>
#include <QPair>

#include "unit.h"
#include "unitmodel.h"
#include "convertoroperator.h"
#include "widgets/gmtoolbox/gamemastertool.h"
namespace Ui {
class Convertor;
}
namespace GMTOOL
{
/**
 * @page Convertor Unit Convertor
 *
 * @section Intro Introduction
 * Convertor provides convertion for any kind of unit. This tool is dedicated to GM to convert value from books.<br/>
 *
 * @section unit Supported Units:
 * @subsection distance Distance Units:
 * @subsection temp Temperature Units:
 * @subsection currency Currency Units:
 *
 *
 */

class Convertor : public GameMasterTool
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
