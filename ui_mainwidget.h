/********************************************************************************
** Form generated from reading UI file 'mainwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChartWidget
{
public:
    QListView *listView;
    QPushButton *submitButton;
    QPushButton *cancelButton;

    void setupUi(QWidget *ChartWidget)
    {
        if (ChartWidget->objectName().isEmpty())
            ChartWidget->setObjectName(QStringLiteral("ChartWidget"));
        ChartWidget->resize(497, 452);
        listView = new QListView(ChartWidget);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setGeometry(QRect(50, 50, 401, 321));
        submitButton = new QPushButton(ChartWidget);
        submitButton->setObjectName(QStringLiteral("submitButton"));
        submitButton->setGeometry(QRect(300, 410, 75, 23));
        cancelButton = new QPushButton(ChartWidget);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setGeometry(QRect(400, 410, 75, 23));

        retranslateUi(ChartWidget);

        QMetaObject::connectSlotsByName(ChartWidget);
    } // setupUi

    void retranslateUi(QWidget *ChartWidget)
    {
        ChartWidget->setWindowTitle(QApplication::translate("ChartWidget", "Form", nullptr));
        submitButton->setText(QApplication::translate("ChartWidget", "OK", nullptr));
        cancelButton->setText(QApplication::translate("ChartWidget", "Back", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChartWidget: public Ui_ChartWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H
