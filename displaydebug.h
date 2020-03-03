#ifndef DISPLAYDEBUG_H
#define DISPLAYDEBUG_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLayout>
#include <QTimer>
#include <QDebug>
#include "table.h"
#include <QSqlTableModel>
#include <QTableView>
#include "TableView.h"
#include "sqlmodule.h"

class DisplayDebug : public QMainWindow
{
    Q_OBJECT
public:
    explicit DisplayDebug(QWidget *parent = nullptr);


    QWidget *centerWindow;
    QTimer * updateTimer;

    /*总的tab*/
    QTabWidget * page;  //组织用的tab

    /*http通信相关*/
    QLabel urlLabel;
    QLabel urlGuid;
    QLineEdit urlText;
    QTextEdit urlGuidText;
    QPushButton btnGetMission;

    QLabel messageLabel;
    QLabel missionIdLabel;
    QLabel para1Label;
    QLabel pare2Label;
    QLabel priorityLabel;
    QLineEdit messageText;
    QLineEdit missionIdText;
    QLineEdit para1Text;
    QLineEdit para2Text;
    QLineEdit priorityText;
    QTextEdit misssionQueueText;
    QPushButton btnPostMissionQueue;
    QPushButton btnDeleteMissionQueue;
    QPushButton btnGetMissionQueue;

    QLabel positionLabel;
    QLabel stateIdLabel;
    QLabel stateTextLabel;
    QLineEdit positionText;
    QLineEdit stateIdText;
    QLineEdit stateTextText;
    QPushButton btnGetStatus;

    QLabel setStateIdLabel;
    QLabel positionLabel_1;
    QLabel stateIdLabel_1;
    QLabel stateTextLabel_1;
    QLineEdit setStateIdText;
    QLineEdit positionText_1;
    QLineEdit stateIdText_1;
    QLineEdit stateTextText_1;
    QPushButton btnPutStatus;

    QLabel positionGuidLabel;
    QTextEdit positionGuidText;
    QPushButton btnGetPosition;

    QHBoxLayout * layout1;
    QHBoxLayout * layout2;
    QHBoxLayout * layout3;
    QHBoxLayout * layout4;
    QHBoxLayout * layout5;

    QVBoxLayout * layout6;

    /*数据库view相关*/
    TableView * sqlView;
signals:

public slots:
    void UpdateValue();

    void SetSqlModel(QSqlTableModel * demoModel);
    void SetSqlView(TableView * demoView);
    void SetSqlWidget(QWidget * demoWidget,QString tableName);
};

#endif // DISPLAYDEBUG_H
