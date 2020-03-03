#ifndef RMSMONITOR_H
#define RMSMONITOR_H

#include <QMainWindow>

namespace Ui {
class RMSMonitor;
}

class RMSMonitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit RMSMonitor(QWidget *parent = 0);
    ~RMSMonitor();

private:
    Ui::RMSMonitor *ui;
};

#endif // RMSMONITOR_H
