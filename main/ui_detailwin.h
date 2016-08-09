/********************************************************************************
** Form generated from reading UI file 'detailwin.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DETAILWIN_H
#define UI_DETAILWIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_detailWin
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTableWidget *mainTable;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *detailWin)
    {
        if (detailWin->objectName().isEmpty())
            detailWin->setObjectName(QStringLiteral("detailWin"));
        detailWin->resize(800, 600);
        centralwidget = new QWidget(detailWin);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mainTable = new QTableWidget(centralwidget);
        mainTable->setObjectName(QStringLiteral("mainTable"));

        verticalLayout->addWidget(mainTable);

        detailWin->setCentralWidget(centralwidget);
        menubar = new QMenuBar(detailWin);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 23));
        detailWin->setMenuBar(menubar);
        statusbar = new QStatusBar(detailWin);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        detailWin->setStatusBar(statusbar);

        retranslateUi(detailWin);

        QMetaObject::connectSlotsByName(detailWin);
    } // setupUi

    void retranslateUi(QMainWindow *detailWin)
    {
        detailWin->setWindowTitle(QApplication::translate("detailWin", "MainWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class detailWin: public Ui_detailWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DETAILWIN_H
