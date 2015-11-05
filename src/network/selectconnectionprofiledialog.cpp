#include "selectconnectionprofiledialog.h"
#include "ui_selectconnectionprofiledialog.h"

SelectConnectionProfileDialog::SelectConnectionProfileDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SelectConnectionProfileDialog)
{
	ui->setupUi(this);
}

SelectConnectionProfileDialog::~SelectConnectionProfileDialog()
{
	delete ui;
}
