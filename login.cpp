#include "login.h"
#include "ui_login.h"
#include "table.h"
#include <QMessageBox>

LogIn::LogIn(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogIn)
{
    ui->setupUi(this);

    connect(ui->okButton,SIGNAL(clicked(bool)),
            this, SLOT(checkValid()));
}
void LogIn::checkValid()
{
    if(ui->usrNameText->toPlainText() == "admin" &&
            ui->passwordText->toPlainText() == "123456")
    {
        //设置valid变量为1
        loginValid = 1;
    }
    else
    {
        //设置valid变量为0
        loginValid = 0;
        QMessageBox::information(NULL, "提示", "用户名或密码错误",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}
LogIn::~LogIn()
{
    delete ui;
}
