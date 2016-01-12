#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QMainWindow>
#include "canvas.h"

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
    void setCurrentTool();
protected:

    bool eventFilter(QObject *, QEvent *);

private:
    Ui::MainWindow *ui;
    Canvas* m_canvas;
    QGraphicsView* m_view;
    QList<Field*> m_fieldList;
    EDITION_TOOL m_currentTool;
    QPoint m_startField;
    QPixmap m_pix;
};

#endif // MAINWINDOW_H
