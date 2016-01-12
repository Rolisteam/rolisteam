#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QMainWindow>
#include "canvas.h"
#include "fieldmodel.h"

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

    void save();
protected:

    bool eventFilter(QObject *, QEvent *);

private:
    Ui::MainWindow *ui;
    Canvas* m_canvas;
    QGraphicsView* m_view;
    EDITION_TOOL m_currentTool;
    QPoint m_startField;
    QPixmap m_pix;
    FieldModel* m_model;
};

#endif // MAINWINDOW_H
