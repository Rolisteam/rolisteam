#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QtDesigner/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT PathChooser : public QWidget
{
    Q_OBJECT
public:
    explicit PathChooser(QWidget *parent = 0);

    QString getPath();
    void setTitle(QString& t);
    void setFilters(QString& t);

signals:
    void pathChanged();

public slots:
    void setPath(QString m);
private slots:
    void onButton();
    void editPath();
private:
    void setupUi();

    QString m_path;
    QLineEdit* m_line;
    QPushButton* m_button;

    QHBoxLayout* m_layout;

    bool m_isDirectory;
    QString m_filters;

    QString m_title;
};

#endif // PATHCHOOSER_H
