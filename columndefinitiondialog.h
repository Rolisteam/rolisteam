#ifndef COLUMNDEFINITIONDIALOG_H
#define COLUMNDEFINITIONDIALOG_H

#include <QDialog>

#include "fieldmodel.h"

class HandleItem;
namespace Ui {
class ColumnDefinitionDialog;
}

class ColumnDefinitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnDefinitionDialog(QWidget *parent = 0);
    ~ColumnDefinitionDialog();


    FieldModel *model() const;
    void setModel(FieldModel *model);

    void load(QJsonObject &json, QList<QGraphicsScene *> scene);
    void save(QJsonObject &json);

public slots:
    void setData(QList<HandleItem*> ,qreal widthTotal,int line, qreal height);
private:
    Ui::ColumnDefinitionDialog *ui;
    FieldModel* m_model;
};

#endif // COLUMNDEFINITIONDIALOG_H
