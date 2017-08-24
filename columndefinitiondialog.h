#ifndef COLUMNDEFINITIONDIALOG_H
#define COLUMNDEFINITIONDIALOG_H

#include <QDialog>

namespace Ui {
class ColumnDefinitionDialog;
}

class ColumnDefinitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnDefinitionDialog(QWidget *parent = 0);
    ~ColumnDefinitionDialog();

private:
    Ui::ColumnDefinitionDialog *ui;
};

#endif // COLUMNDEFINITIONDIALOG_H
