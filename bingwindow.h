#ifndef BINGWINDOW_H
#define BINGWINDOW_H

#include <QWidget>

namespace Ui {
class BingWindow;
}

class BingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BingWindow(QWidget *parent = 0);
    ~BingWindow();

private:
    Ui::BingWindow *ui;
};

#endif // BINGWINDOW_H
