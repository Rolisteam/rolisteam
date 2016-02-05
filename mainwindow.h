#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QMainWindow>
#include "canvas.h"
#include "fieldmodel.h"
#include "rolisteamimageprovider.h"
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


    Field* addFieldAt(QPoint pos);
    bool qmlGeneration() const;
    void setQmlGeneration(bool qmlGeneration);

public slots:
    void setCurrentTool();

    void save();
    void saveAs();

    void open();
    void generateQML(QString& qml);

    void exportToRolisteam();

    void showQML();
    void showQMLFromCode();

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
    QString m_filename;
    bool m_qmlGeneration;
    RolisteamImageProvider* m_imgProvider;

};

#endif // MAINWINDOW_H
