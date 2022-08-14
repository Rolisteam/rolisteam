#ifndef CODEEDIT_H
#define CODEEDIT_H

#include "charactersheet/controllers/fieldcontroller.h"
#include <QDialog>

namespace Ui
{
class CodeEdit;
}

class CodeEdit : public QDialog
{
    Q_OBJECT

public:
    explicit CodeEdit(FieldController* m_field, QWidget* parent= 0);
    ~CodeEdit();

private:
    Ui::CodeEdit* ui;
    FieldController* m_field;
};

#endif // CODEEDIT_H
