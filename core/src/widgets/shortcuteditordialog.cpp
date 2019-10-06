#include "shortcuteditordialog.h"
#include "ui_shortcuteditordialog.h"

#include "data/shortcutmodel.h"

ShortCutEditorDialog::ShortCutEditorDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ShortCutEditorDialog)
{
    ui->setupUi(this);
}

ShortCutEditorDialog::~ShortCutEditorDialog()
{
    delete ui;
}

QAbstractItemModel* ShortCutEditorDialog::model() const
{
    return m_model;
}

void ShortCutEditorDialog::setModel(QAbstractItemModel* model)
{
    m_model= model;
    ui->m_shortCutView->setModel(m_model);
    ui->m_shortCutView->expandAll();
}
