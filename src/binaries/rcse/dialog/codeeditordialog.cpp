#include "codeeditordialog.h"
#include "ui_codeeditordialog.h"

CodeEditorDialog::CodeEditorDialog(QWidget* parent) : QDialog(parent), ui(new Ui::CodeEditorDialog)
{
    ui->setupUi(this);

    m_syntaxHighlight.reset(new QmlHighlighter(ui->m_codeEditor->document()));
    m_syntaxHighlight->setObjectName("HighLighterForQML");
    connect(this, &CodeEditorDialog::plainTextChanged, ui->m_codeEditor, &CodeEditor::textChanged);
}

CodeEditorDialog::~CodeEditorDialog()
{
    delete ui;
}
void CodeEditorDialog::setPlainText(const QString data)
{
    if(ui->m_codeEditor->toPlainText() == data)
        return;
    ui->m_codeEditor->setPlainText(data);
    emit plainTextChanged();
}
QString CodeEditorDialog::toPlainText() const
{
    return ui->m_codeEditor->toPlainText();
}
