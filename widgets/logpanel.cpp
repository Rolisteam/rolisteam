#include "logpanel.h"
#include "ui_logpanel.h"

#include <QFileDialog>
#include <QTextStream>
/////////////////////////////////////
//
// LogPanel code
//
/////////////////////////////////////
LogPanel::LogPanel(LogController* controller, QWidget* parent)
    : QWidget(parent), ui(new Ui::LogPanel), m_controller(controller)
{
    ui->setupUi(this);
    ui->m_eraseBtn->setDefaultAction(ui->m_eraseAllAct);
    ui->m_saveBtn->setDefaultAction(ui->m_saveAct);

    // QIcon::fromTheme("edit-clear",)
    ui->m_eraseAllAct->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->m_saveAct->setIcon(QIcon::fromTheme("document-save", QIcon(":/resources/icons/save.png")));

    connect(ui->m_logLevel, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]() {
        auto logLevel= static_cast<LogController::LogLevel>(ui->m_logLevel->currentIndex());
        controller->setLogLevel(logLevel);
        m_prefManager->registerValue("LogController_LogLevel", ui->m_logLevel->currentIndex());
    });

    connect(m_controller, &LogController::showMessage, this, &LogPanel::showMessage);
}

LogPanel::~LogPanel()
{
    delete ui;
}

void LogPanel::showMessage(QString msg, LogController::LogLevel level)
{
    static bool alternance= false;

    QColor color;
    alternance= !alternance;

    if(alternance)
        color= Qt::darkBlue;
    else
        color= Qt::darkRed;

    if(level == LogController::Error)
    {
        color= Qt::red;
    }
    if(level == LogController::Warning)
    {
        color= Qt::darkRed;
    }
    ui->m_logview->setTextColor(color);
    ui->m_logview->append(msg);
}

void LogPanel::initSetting()
{
    if(m_controller == nullptr || m_prefManager == nullptr)
        return;

    ui->m_logLevel->setCurrentIndex(m_prefManager->value("LogController_LogLevel", 0).toInt());
    auto logLevel= static_cast<LogController::LogLevel>(ui->m_logLevel->currentIndex());
    m_controller->setLogLevel(logLevel);
}

void LogPanel::saveLog()
{
    auto filePath
        = QFileDialog::getSaveFileName(this, tr("Saving logs"), QDir::homePath(), tr("Log files %1").arg("(*.rlog)"));
    QFile file(filePath);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream in(&file);
        in << ui->m_logview->toPlainText();
    }
}
