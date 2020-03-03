#include "rmsmonitor.h"
#include "ui_rmsmonitor.h"

RMSMonitor::RMSMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RMSMonitor)
{
    ui->setupUi(this);
}

RMSMonitor::~RMSMonitor()
{
    delete ui;
}
