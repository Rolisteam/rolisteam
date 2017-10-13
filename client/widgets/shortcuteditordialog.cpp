#include "shortcuteditordialog.h"
#include "ui_shortcuteditordialog.h"

#include "data/shortcutmodel.h"

ShortCutEditorDialog::ShortCutEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortCutEditorDialog)
{
    ui->setupUi(this);
	m_model = new ShortCutModel();
	ui->m_shortcutView->setModel(m_model);
}

ShortCutEditorDialog::~ShortCutEditorDialog()
{
    delete ui;
}
