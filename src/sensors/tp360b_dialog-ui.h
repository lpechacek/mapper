/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QScrollArea>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QPushButton *connectButton;
    QHBoxLayout *infoStripLayout;
    QProgressBar *batteryCharge;
    QLabel *fwId;
    QPushButton *disconnectButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QRadioButton *placeDot;
    QRadioButton *drawLine;
    QRadioButton *placeVertex;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *confirmData;
    QPushButton *confirmDataButton;
    QVBoxLayout *verticalLayout;
    QCheckBox *showDebugBox;
    QTextEdit *logWidget;
    QSpacerItem *verticalSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QScrollArea *MainWindow)
    {
        verticalLayout_2 = new QVBoxLayout(MainWindow);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        verticalLayout_2->addWidget(connectButton);

        infoStripLayout = new QHBoxLayout();
        infoStripLayout->setObjectName(QString::fromUtf8("infoStripLayout"));
        batteryCharge = new QProgressBar(centralwidget);
        batteryCharge->setObjectName(QString::fromUtf8("batteryCharge"));
        batteryCharge->setMinimum(2000);
        batteryCharge->setMaximum(3000);

        infoStripLayout->addWidget(batteryCharge);

        fwId = new QLabel(centralwidget);
        fwId->setObjectName(QString::fromUtf8("fwId"));

        infoStripLayout->addWidget(fwId);

        disconnectButton = new QPushButton(centralwidget);
        disconnectButton->setObjectName(QString::fromUtf8("disconnectButton"));

        infoStripLayout->addWidget(disconnectButton);


        verticalLayout_2->addLayout(infoStripLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        placeDot = new QRadioButton(centralwidget);
        placeDot->setObjectName(QString::fromUtf8("placeDot"));
        placeDot->setChecked(true);

        horizontalLayout_2->addWidget(placeDot);

        drawLine = new QRadioButton(centralwidget);
        drawLine->setObjectName(QString::fromUtf8("drawLine"));

        horizontalLayout_2->addWidget(drawLine);

        placeVertex = new QRadioButton(centralwidget);
        placeVertex->setObjectName(QString::fromUtf8("placeObject"));

        horizontalLayout_2->addWidget(placeVertex);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        confirmData = new QCheckBox(centralwidget);
        confirmData->setObjectName(QString::fromUtf8("confirmData"));

        horizontalLayout_3->addWidget(confirmData);

        confirmDataButton = new QPushButton(centralwidget);
        confirmDataButton->setObjectName(QString::fromUtf8("confirmDataButton"));
        confirmDataButton->setEnabled(false);

        horizontalLayout_3->addWidget(confirmDataButton);


        verticalLayout_2->addLayout(horizontalLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        showDebugBox = new QCheckBox(centralwidget);
        showDebugBox->setObjectName(QString::fromUtf8("showDebugBox"));
		showDebugBox->setChecked(true);

        verticalLayout->addWidget(showDebugBox);

        logWidget = new QTextEdit(centralwidget);
        logWidget->setObjectName(QString::fromUtf8("logWidget"));
        logWidget->setFocusPolicy(Qt::NoFocus);
        logWidget->setAcceptDrops(false);

        verticalLayout->addWidget(logWidget);

		verticalLayout_2->addLayout(verticalLayout);

		verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

		verticalLayout_2->addItem(verticalSpacer);

        retranslateUi(MainWindow);
		QObject::connect(showDebugBox, &QCheckBox::toggled, [this](bool debug_enabled) {
			logWidget->setVisible(debug_enabled);
			verticalSpacer->changeSize(0, 0, QSizePolicy::Minimum,
			                           debug_enabled ? QSizePolicy::Minimum : QSizePolicy::Expanding);
		});
        QObject::connect(confirmData, SIGNAL(toggled(bool)), confirmDataButton, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QScrollArea *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        batteryCharge->setFormat(QCoreApplication::translate("MainWindow", "%vmV (%p%)", nullptr));
        fwId->setText(QCoreApplication::translate("MainWindow", "FW ???", nullptr));
        disconnectButton->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Draw:", nullptr));
        placeDot->setText(QCoreApplication::translate("MainWindow", "Dot", nullptr));
        drawLine->setText(QCoreApplication::translate("MainWindow", "Line", nullptr));
        placeVertex->setText(QCoreApplication::translate("MainWindow", "Vertex", nullptr));
        confirmData->setText(QCoreApplication::translate("MainWindow", "Confirm measurements", nullptr));
        confirmDataButton->setText(QCoreApplication::translate("MainWindow", "Confirm", nullptr));
        showDebugBox->setText(QCoreApplication::translate("MainWindow", "Show debugging data", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
