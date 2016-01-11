#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMimeData>
#include <QUrl>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setAcceptDrops(true);
    ui->setupUi(this);

    m_picLabel = new QLabel();
    m_picLabel->installEventFilter(this);
    ui->scrollArea->setWidget(m_picLabel);

    ui->m_splitter->setStretchFactor(0,1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* data = event->mimeData();
    if(data->hasUrls())
    {
        QList<QUrl> list = data->urls();
        foreach(QUrl url, list)
        {
            QImage imag(url.toLocalFile());
            if(!imag.isNull())
            {
                m_picLabel->setPixmap(QPixmap(url.toLocalFile()));
            }
        }
        event->acceptProposedAction();
    }
}
void MainWindow::dragEnterEvent(QDragEnterEvent * event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }

}
bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if(obj == m_picLabel)
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
    }
    return QMainWindow::eventFilter(obj,ev);
}
Field* MainWindow::addFieldAt(QPoint pos)
{
    qDebug() << "create Field";
    Field* field = new Field(m_picLabel);
    m_fieldList.append(field);
    field->move(pos);
    field->setPosition(pos);
    field->drawField();
    field->setVisible(true);
    connect(field,SIGNAL(clickOn(Field*)),this,SLOT(updateEditorPanel(Field*)));
    updateEditorPanel(field);
    return field;
}

void MainWindow::updateEditorPanel(Field* f)
{
    if(NULL!=f)
    {
        ui->m_xPos->setValue(f->pos().x());
        ui->m_yPos->setValue(f->pos().y());
        ui->m_width->setValue(f->size().width());
        ui->m_height->setValue(f->size().height());
        ui->m_key->setText(f->key());
    }
}
