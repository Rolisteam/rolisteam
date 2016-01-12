#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMimeData>
#include <QUrl>
#include <QOpenGLWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QBuffer>
#include <QJsonDocument>
#include "borderlisteditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setAcceptDrops(true);
    ui->setupUi(this);

    m_canvas = new Canvas();
    m_model = new FieldModel();
    ui->treeView->setModel(m_model);
    m_canvas->setModel(m_model);
    ui->treeView->setItemDelegateForColumn(Item::BORDER,new BorderListEditor);
    m_view = new QGraphicsView();
    m_view->setAcceptDrops(true);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setViewport(new QOpenGLWidget());
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );


    m_view->setScene(m_canvas);
    //m_view->installEventFilter(this);
    ui->scrollArea->setWidget(m_view);

    ui->m_splitter->setStretchFactor(0,1);

    ui->m_addAct->setData(Canvas::ADD);
    ui->m_moveAct->setData(Canvas::MOVE);
    ui->m_deleteAct->setData(Canvas::DELETE);

    ui->m_addBtn->setDefaultAction(ui->m_addAct);
    ui->m_moveBtn->setDefaultAction(ui->m_moveAct);
    ui->m_deleteBtn->setDefaultAction(ui->m_deleteAct);


    connect(ui->m_addAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_moveAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_deleteAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));

    connect(ui->m_saveAct,SIGNAL(triggered(bool)),this,SLOT(save()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setCurrentTool()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    m_canvas->setCurrentTool((Canvas::Tool)action->data().toInt());
}
void MainWindow::save()
{
    QString name = QFileDialog::getSaveFileName(this,tr("Save CharacterSheet"),QDir::homePath(),tr("Rolisteam CharacterSheet (*.rcs)"));

    if(!name.isNull())
    {
        QJsonDocument json;
        QJsonObject jsonObj;
        m_model->save(jsonObj);
        QPixmap pix = m_canvas->pixmap();
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pix.save(&buffer, "PNG");
        jsonObj["image"]=QString(buffer.data().toBase64());
        json.setObject(jsonObj);


        QFile file(name);
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(json.toJson());
        }
        else
        {
            QMessageBox::information(this,tr("Not Readable"), tr("The selected file is not readable"));
        }
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
  /*  if(obj == m_picLabel)
    {
        if(ev->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* event = static_cast<QMouseEvent*>(ev);
            m_startField = event->pos();

            return true;
        }
        else if(ev->type() == QEvent::MouseButtonRelease)
        {
                QMouseEvent* event = static_cast<QMouseEvent*>(ev);
                if(event->modifiers() == Qt::NoModifier)
                {
                    Field* field = addFieldAt(m_startField);
                QPoint point = event->pos();
                field->setGeometry(m_startField.x(),m_startField.y(),m_startField.x()+point.x(),m_startField.y()+point.y());

                return true;
                }
        }
    }*/
    return QMainWindow::eventFilter(obj,ev);
}
Field* MainWindow::addFieldAt(QPoint pos)
{
    qDebug() << "create Field";
   /* Field* field = new Field(m_picLabel);
    m_fieldList.append(field);
    field->move(pos);
    field->setPosition(pos);
    field->drawField();
    field->setVisible(true);
    connect(field,SIGNAL(clickOn(Field*)),this,SLOT(updateEditorPanel(Field*)));
    updateEditorPanel(field);
    return field;*/
    return NULL;
}

/*void MainWindow::updateEditorPanel(Field* f)
{
    if(NULL!=f)
    {
        ui->m_xPos->setValue(f->pos().x());
        ui->m_yPos->setValue(f->pos().y());
        ui->m_width->setValue(f->size().width());
        ui->m_height->setValue(f->size().height());
        ui->m_key->setText(f->key());
    }
}*/
