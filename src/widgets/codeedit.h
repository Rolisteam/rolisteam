#ifndef CODEEDIT_H
#define CODEEDIT_H

#include <QDialog>
#include "field.h"

namespace Ui {
class CodeEdit;
}

class CodeEdit : public QDialog
{
    Q_OBJECT

public:
    explicit CodeEdit(Field* m_field, QWidget *parent = 0);
    ~CodeEdit();

private:
    Ui::CodeEdit *ui;
    Field* m_field;
};

#endif // CODEEDIT_H
