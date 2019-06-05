#ifndef CODEEDITORDIALOG_H
#define CODEEDITORDIALOG_H

#include <QDialog>
#include <memory>
#include <qmlhighlighter.h>

namespace Ui
{
    class CodeEditorDialog;
}

class CodeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeEditorDialog(QWidget* parent= nullptr);
    ~CodeEditorDialog();

    void setPlainText(const QString data);
    QString toPlainText();

private:
    Ui::CodeEditorDialog* ui;
    std::unique_ptr<QmlHighlighter> m_syntaxHighlight;
};

#endif // CODEEDITORDIALOG_H
