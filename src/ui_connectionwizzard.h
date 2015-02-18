/********************************************************************************
** Form generated from reading UI file 'connectionwizzard.ui'
**
** Created: Sun Aug 15 13:04:31 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTIONWIZZARD_H
#define UI_CONNECTIONWIZZARD_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConnectionWizzard
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QListWidget *m_connectionListWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_2;
    QLineEdit *m_name;
    QLabel *label;
    QLineEdit *m_hostname;
    QLabel *label_3;
    QSpinBox *m_port;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ConnectionWizzard)
    {
        if (ConnectionWizzard->objectName().isEmpty())
            ConnectionWizzard->setObjectName(QString::fromUtf8("ConnectionWizzard"));
        ConnectionWizzard->resize(469, 286);
        ConnectionWizzard->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        horizontalLayout_2 = new QHBoxLayout(ConnectionWizzard);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        m_connectionListWidget = new QListWidget(ConnectionWizzard);
        m_connectionListWidget->setObjectName(QString::fromUtf8("m_connectionListWidget"));
        m_connectionListWidget->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        verticalLayout_3->addWidget(m_connectionListWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        m_addButton = new QPushButton(ConnectionWizzard);
        m_addButton->setObjectName(QString::fromUtf8("m_addButton"));

        horizontalLayout->addWidget(m_addButton);

        m_deleteButton = new QPushButton(ConnectionWizzard);
        m_deleteButton->setObjectName(QString::fromUtf8("m_deleteButton"));

        horizontalLayout->addWidget(m_deleteButton);


        verticalLayout_3->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(ConnectionWizzard);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        m_name = new QLineEdit(groupBox);
        m_name->setObjectName(QString::fromUtf8("m_name"));

        formLayout->setWidget(0, QFormLayout::FieldRole, m_name);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        m_hostname = new QLineEdit(groupBox);
        m_hostname->setObjectName(QString::fromUtf8("m_hostname"));

        formLayout->setWidget(1, QFormLayout::FieldRole, m_hostname);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        m_port = new QSpinBox(groupBox);
        m_port->setObjectName(QString::fromUtf8("m_port"));
        m_port->setMaximum(65535);
        m_port->setValue(6660);

        formLayout->setWidget(2, QFormLayout::FieldRole, m_port);


        verticalLayout->addLayout(formLayout);


        verticalLayout_2->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(ConnectionWizzard);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        horizontalLayout_2->addLayout(verticalLayout_2);

        horizontalLayout_2->setStretch(1, 1);

        retranslateUi(ConnectionWizzard);

        QMetaObject::connectSlotsByName(ConnectionWizzard);
    } // setupUi

    void retranslateUi(QDialog *ConnectionWizzard)
    {
        ConnectionWizzard->setWindowTitle(QApplication::translate("ConnectionWizzard", "Dialog", 0, QApplication::UnicodeUTF8));
        m_addButton->setText(QApplication::translate("ConnectionWizzard", "Add", 0, QApplication::UnicodeUTF8));
        m_deleteButton->setText(QApplication::translate("ConnectionWizzard", "Delete", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ConnectionWizzard", "Connection", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ConnectionWizzard", "Name :", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ConnectionWizzard", "Hostname/Address IP:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ConnectionWizzard", "Port :", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConnectionWizzard: public Ui_ConnectionWizzard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTIONWIZZARD_H
