#include "onlinepicturedialog.h"
#include "ui_onlinepicturedialog.h"

OnlinePictureDialog::OnlinePictureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OnlinePictureDialog)
{
    ui->setupUi(this);
}

OnlinePictureDialog::~OnlinePictureDialog()
{
    delete ui;
}
