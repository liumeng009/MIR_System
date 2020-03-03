/********************************************************************************
** Form generated from reading UI file 'sysconfig.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYSCONFIG_H
#define UI_SYSCONFIG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SysConfig
{
public:
    QFrame *frame;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *IPText;
    QLineEdit *portText;
    QPushButton *IPOkButton;
    QPushButton *portOkButton;
    QFrame *frame_2;
    QLabel *label_3;
    QPushButton *manaCodeOkButton;
    QLineEdit *manaCodeText;
    QLineEdit *opCodeText;
    QPushButton *opCodeOkButton;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *openCodeText;
    QPushButton *openCodeOkButton;

    void setupUi(QWidget *SysConfig)
    {
        if (SysConfig->objectName().isEmpty())
            SysConfig->setObjectName(QStringLiteral("SysConfig"));
        SysConfig->resize(448, 394);
        frame = new QFrame(SysConfig);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(10, 10, 421, 151));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 80, 54, 12));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 40, 54, 12));
        IPText = new QLineEdit(frame);
        IPText->setObjectName(QStringLiteral("IPText"));
        IPText->setGeometry(QRect(90, 40, 221, 21));
        portText = new QLineEdit(frame);
        portText->setObjectName(QStringLiteral("portText"));
        portText->setGeometry(QRect(90, 80, 113, 20));
        IPOkButton = new QPushButton(frame);
        IPOkButton->setObjectName(QStringLiteral("IPOkButton"));
        IPOkButton->setGeometry(QRect(330, 40, 75, 23));
        portOkButton = new QPushButton(frame);
        portOkButton->setObjectName(QStringLiteral("portOkButton"));
        portOkButton->setGeometry(QRect(330, 80, 75, 23));
        frame_2 = new QFrame(SysConfig);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setGeometry(QRect(10, 170, 421, 191));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 40, 54, 12));
        manaCodeOkButton = new QPushButton(frame_2);
        manaCodeOkButton->setObjectName(QStringLiteral("manaCodeOkButton"));
        manaCodeOkButton->setGeometry(QRect(330, 40, 75, 23));
        manaCodeText = new QLineEdit(frame_2);
        manaCodeText->setObjectName(QStringLiteral("manaCodeText"));
        manaCodeText->setGeometry(QRect(90, 40, 221, 21));
        opCodeText = new QLineEdit(frame_2);
        opCodeText->setObjectName(QStringLiteral("opCodeText"));
        opCodeText->setGeometry(QRect(90, 80, 221, 20));
        opCodeOkButton = new QPushButton(frame_2);
        opCodeOkButton->setObjectName(QStringLiteral("opCodeOkButton"));
        opCodeOkButton->setGeometry(QRect(330, 80, 75, 23));
        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 80, 54, 12));
        label_5 = new QLabel(frame_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 120, 54, 12));
        openCodeText = new QLineEdit(frame_2);
        openCodeText->setObjectName(QStringLiteral("openCodeText"));
        openCodeText->setGeometry(QRect(90, 120, 221, 20));
        openCodeOkButton = new QPushButton(frame_2);
        openCodeOkButton->setObjectName(QStringLiteral("openCodeOkButton"));
        openCodeOkButton->setGeometry(QRect(330, 120, 75, 23));

        retranslateUi(SysConfig);

        QMetaObject::connectSlotsByName(SysConfig);
    } // setupUi

    void retranslateUi(QWidget *SysConfig)
    {
        SysConfig->setWindowTitle(QApplication::translate("SysConfig", "\347\263\273\347\273\237\347\233\270\345\205\263\351\205\215\347\275\256", nullptr));
        label_2->setText(QApplication::translate("SysConfig", "\347\233\221\345\220\254\347\253\257\345\217\243", nullptr));
        label->setText(QApplication::translate("SysConfig", "\347\233\221\345\220\254 IP", nullptr));
        IPOkButton->setText(QApplication::translate("SysConfig", "OK", nullptr));
        portOkButton->setText(QApplication::translate("SysConfig", "OK", nullptr));
        label_3->setText(QApplication::translate("SysConfig", "\347\256\241\347\220\206\345\257\206\347\240\201", nullptr));
        manaCodeOkButton->setText(QApplication::translate("SysConfig", "OK", nullptr));
        opCodeOkButton->setText(QApplication::translate("SysConfig", "OK", nullptr));
        label_4->setText(QApplication::translate("SysConfig", "\346\223\215\344\275\234\345\257\206\347\240\201", nullptr));
        label_5->setText(QApplication::translate("SysConfig", "\345\274\200\344\273\223\345\257\206\347\240\201", nullptr));
        openCodeOkButton->setText(QApplication::translate("SysConfig", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SysConfig: public Ui_SysConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYSCONFIG_H
