#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include "field.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum EDITION_TOOL {ADDFIELD,SELECT};
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    Field *addFieldAt(QPoint pos);
public slots:
    void updateEditorPanel(Field*);
protected:
    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent * event);

    bool eventFilter(QObject *, QEvent *);

private:
    Ui::MainWindow *ui;
    QLabel* m_picLabel;
    QList<Field*> m_fieldList;
    EDITION_TOOL m_currentTool;
    QPoint m_startField;
};

#endif // MAINWINDOW_H
