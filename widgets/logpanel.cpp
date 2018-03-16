#include "logpanel.h"
#include "ui_logpanel.h"


#include <QFileDialog>
#include <QTextStream>




LogPanel::LogPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogPanel)
{
    ui->setupUi(this);
    ui->m_eraseBtn->setDefaultAction(ui->m_eraseAllAct);
    ui->m_saveBtn->setDefaultAction(ui->m_saveAct);

    connect(ui->m_logLevel,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[=](){
        emit logLevelChanged(static_cast<LogController::LogLevel>(ui->m_logLevel->currentIndex()));
    });
}

LogPanel::~LogPanel()
{
    delete ui;
}

void LogPanel::showMessage(QString msg)
{
    ui->m_logview->append(msg);
}

void LogPanel::saveLog()
{
    auto filePath = QFileDialog::getSaveFileName(this,tr("Saving logs"),QDir::homePath(),tr("Log files %1").arg("(*.rlog)"));
    QFile file(filePath);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream in(&file);
        in << ui->m_logview->toPlainText();
    }
}

