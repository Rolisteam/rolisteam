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
    Q_PROPERTY(QString plainText READ toPlainText WRITE setPlainText NOTIFY plainTextChanged FINAL)
public:
    explicit CodeEditorDialog(QWidget* parent= nullptr);
    ~CodeEditorDialog();

    QString toPlainText() const;
public slots:
    void setPlainText(const QString data);
signals:
    void plainTextChanged();

private:
    Ui::CodeEditorDialog* ui;
    std::unique_ptr<QmlHighlighter> m_syntaxHighlight;
};

#endif // CODEEDITORDIALOG_H
