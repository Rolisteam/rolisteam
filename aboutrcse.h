#ifndef ABOUTRCSE_H
#define ABOUTRCSE_H

#include <QDialog>

namespace Ui {
class AboutRcse;
}

class AboutRcse : public QDialog
{
    Q_OBJECT

public:
    explicit AboutRcse(QString verison, QWidget *parent);
    ~AboutRcse();

    QString version() const;
    void setVersion(const QString &version);

private:
    Ui::AboutRcse *ui;
    QString m_version;
};

#endif // ABOUTRCSE_H
