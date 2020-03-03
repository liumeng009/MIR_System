#include "bingwindow.h"
#include "ui_bingwindow.h"

BingWindow::BingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BingWindow)
{
    ui->setupUi(this);
}

BingWindow::~BingWindow()
{
    delete ui;
}
