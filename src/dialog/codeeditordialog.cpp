#include "codeeditordialog.h"
#include "ui_codeeditordialog.h"

CodeEditorDialog::CodeEditorDialog(QWidget* parent) : QDialog(parent), ui(new Ui::CodeEditorDialog)
{
    ui->setupUi(this);

    m_syntaxHighlight.reset(new QmlHighlighter(ui->m_codeEditor->document()));
    m_syntaxHighlight->setObjectName("HighLighterForQML");
}

CodeEditorDialog::~CodeEditorDialog()
{
    delete ui;
}
void CodeEditorDialog::setPlainText(const QString data)
{
    ui->m_codeEditor->setPlainText(data);
}
QString CodeEditorDialog::toPlainText()
{
    return ui->m_codeEditor->toPlainText();
}
