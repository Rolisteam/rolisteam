#include "dicebookmarkwidget.h"
#include "ui_dicebookmarkwidget.h"
#include <QHeaderView>
DiceBookMarkWidget::DiceBookMarkWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiceBookMarkWidget)
{
    ui->setupUi(this);
    m_model = new DiceBookMarkModel(this);
    ui->tableView->setModel(m_model);
    auto header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(true);
    connect(ui->m_addBtn,&QToolButton::clicked,this,[=](){
        m_model->appendRows();
    });
    connect(ui->m_removeBtn,&QToolButton::clicked,this,[=](){
        auto index = ui->tableView->currentIndex();
        if(index.isValid())
        {
            m_model->removeRows(index.row(),1);
        }
    });
}

DiceBookMarkWidget::~DiceBookMarkWidget()
{
    delete ui;
}
