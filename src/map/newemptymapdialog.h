#ifndef NEWEMPTYMAPDIALOG_H
#define NEWEMPTYMAPDIALOG_H

#include <QDialog>
#include "widgets/colorbutton.h"

#include "map/Carte.h"


namespace Ui {
class NewEmptyMapDialog;
}

class NewEmptyMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEmptyMapDialog(QWidget *parent = 0);
    ~NewEmptyMapDialog();

    Carte::PermissionMode getPermission() const;
    QString getTitle( ) const;
    QColor getColor( ) const;
    QSize getSize() const;
    static QStringList getPermissionData();

public slots:
    /**
     * @brief updateSize radiobutton given the format choice.
     */
    void updateSize();
    /**
     * @brief resetData- reset data to default
     */
    void resetData();

private:
    Ui::NewEmptyMapDialog *ui;

    QStringList  m_labelModel;
    static QStringList  m_permissionData;
    QList<QSize> m_landscapeModel;
    QList<QSize> m_portraitModel;
    QList<QSize> m_squareModel;
    ColorButton* m_colorButton;
};

#endif // NEWEMPTYMAPDIALOG_H
