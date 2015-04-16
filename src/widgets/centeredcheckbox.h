#ifndef CENTEREDCHECKBOX_H
#define CENTEREDCHECKBOX_H

#include <QWidget>
#include <QCheckBox>
class CenteredCheckBox : public QWidget
{
    Q_OBJECT
 public:
    CenteredCheckBox( QWidget* parent=NULL);

    bool isCheckedDelegate() const;
    void setCheckedDelegate(bool);

signals:
    void	commitEditor();

private:
        QCheckBox* m_editorCheckBox;
};

#endif // CENTEREDCHECKBOX_H
