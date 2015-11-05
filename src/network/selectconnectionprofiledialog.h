#ifndef SELECTCONNECTIONPROFILEDIALOG_H
#define SELECTCONNECTIONPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class SelectConnectionProfileDialog;
}

class SelectConnectionProfileDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SelectConnectionProfileDialog(QWidget *parent = 0);
	~SelectConnectionProfileDialog();

private:
	Ui::SelectConnectionProfileDialog *ui;
};

#endif // SELECTCONNECTIONPROFILEDIALOG_H
