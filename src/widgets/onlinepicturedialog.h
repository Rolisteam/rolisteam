#ifndef ONLINEPICTUREDIALOG_H
#define ONLINEPICTUREDIALOG_H

#include <QDialog>

namespace Ui {
class OnlinePictureDialog;
}

class OnlinePictureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OnlinePictureDialog(QWidget *parent = 0);
    ~OnlinePictureDialog();

private:
    Ui::OnlinePictureDialog *ui;
};

#endif // ONLINEPICTUREDIALOG_H
