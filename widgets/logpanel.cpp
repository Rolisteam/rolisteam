#include "logpanel.h"
#include "ui_logpanel.h"


#include <QFileDialog>
#include <QTextStream>
/////////////////////////////////////
//
// LogPanel code
//
/////////////////////////////////////
LogPanel::LogPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogPanel)
{
    ui->setupUi(this);
    ui->m_eraseBtn->setDefaultAction(ui->m_eraseAllAct);
    ui->m_saveBtn->setDefaultAction(ui->m_saveAct);



    //QIcon::fromTheme("edit-clear",)
    ui->m_eraseAllAct->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->m_saveAct->setIcon(QIcon::fromTheme("document-save", QIcon(":/resources/icons/save.png")));


    connect(ui->m_logLevel,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[=](){
        m_currentLogLevel = static_cast<LogController::LogLevel>(ui->m_logLevel->currentIndex());
        emit logLevelChanged(m_currentLogLevel);
        m_prefManager->registerValue("LogController_LogLevel",ui->m_logLevel->currentIndex());
    });
}

LogPanel::~LogPanel()
{
    delete ui;
}

void LogPanel::showMessage(QString msg,LogController::LogLevel level)
{
    static bool alternance = false;

    if(m_currentLogLevel < level && level !=LogController::Features)
        return;

    QColor color;
    alternance = !alternance;

    if (alternance)
        color = Qt::darkBlue;
    else
        color = Qt::darkRed;

    if(level == LogController::Error)
    {
        color = Qt::red;
    }
    if(level == LogController::Warning)
    {
        color = Qt::darkRed;
    }
    ui->m_logview->setTextColor(color);
    ui->m_logview->append(msg);
}

void LogPanel::initSetting()
{
    ui->m_logLevel->setCurrentIndex(m_prefManager->value("LogController_LogLevel",0).toInt());
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




