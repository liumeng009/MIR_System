#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>
#include <QLocalServer>

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
     SingleApplication(int argc,char **argv);
     ~SingleApplication();

     bool isRunning();

private slots:
     void newLocalConnection();

private:
     QLocalServer *server;

     bool _isRunning;

};

#endif // SINGLEAPPLICATION_H
