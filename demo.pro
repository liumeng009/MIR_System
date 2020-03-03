#-------------------------------------------------
#
# Project created by QtCreator 2019-07-13T16:17:53
#
#-------------------------------------------------

QT       += core gui sql network serialport testlib
CONFIG   += qaxcontainer

#图标小爱
RC_ICONS = xiaoai.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(excel/xlsx/qtxlsx.pri)

SOURCES += \
        main.cpp \
    schedule/baseapi.cpp \
    boxUnit.cpp \
    schedule/cmdstatemachine.cpp \
    delegate.cpp \
    displaydebug.cpp \
    faderwidget.cpp \
    fingermodule.cpp \
    frame.cpp \
    schedule/httpcon.cpp \
    login.cpp \
    schedule/mircmdcon.cpp \
    schedule/mirschedule.cpp \
    mserialport.cpp \
    schedule/requestnode.cpp \
    schedule/requestqueuecon.cpp \
    rms.cpp \
    rmsfactory.cpp \
    rmsmonitor.cpp \
    session.cpp \
    singleapplication.cpp \
    SocketModel.cpp \
    sqlmodule.cpp \
    table.cpp \
    TableModel.cpp \
    TableView.cpp \
    tcpserver.cpp \
    tcpsocket.cpp \
    echo/bingquwindow.cpp \
    echo/fingertoolwindow.cpp \
    echo/miniwindow.cpp \
    echo/mirdebug.cpp \
    echo/monitor.cpp \
    echo/rmswindow.cpp \
    echo/serialdialog.cpp \
    echo/sysconfigWindow.cpp \
    echo/syswindow.cpp \
    echo/xiangtiwindow.cpp \
    echo/yihuwindow.cpp \
    sql/bingqutablecon.cpp \
    sql/boxtablecon.cpp \
    sql/recordtablecon.cpp \
    sql/sqliteutil.cpp \
    sql/yihutablecon.cpp \
    chatserver/chatserver.cpp \
    chatserver/serverworker.cpp \
    applyform/applyform.cpp \
    applyform/applyjson.cpp \
    echoWindow/boxwindow.cpp \
    echoWindow/boxitemdelegte.cpp \
    echoWindow/boxmodel.cpp \
    controller/boxcon.cpp \
    echoWindow/stockwindow.cpp \
    echoWindow/listmodel.cpp \
    echoWindow/MuItemDelegate.cpp \
    echoWindow/cellitem/cellitem.cpp \
    echoWindow/cellitem/draweritem.cpp \
    appconfig/sysconfig.cpp \
    echoWindow/chart/chartmodel.cpp \
    echoWindow/chart/chartitemdelegte.cpp \
    echoWindow/report/ButtonDelegate.cpp \
    echoWindow/report/reportview.cpp \
    echoWindow/report/reportdata.cpp \
    echoWindow/report/reportmodel.cpp \
    echoWindow/reportwindow.cpp \
    echoWindow/stockcontent/stockcontentwindow.cpp \
    controller/reportcon.cpp \
    excel/excelengine.cpp \
    excel/excelnode.cpp \
    controller/excelcon.cpp \
    itemdef.cpp

HEADERS += \
    schedule/baseapi.h \
    boxunit.h \
    schedule/cmdstatemachine.h \
    delegate.h \
    displaydebug.h \
    faderwidget.h \
    fingermodule.h \
    frame.h \
    schedule/httpcon.h \
    login.h \
    schedule/mircmdcon.h \
    schedule/mirschedule.h \
    mserialport.h \
    schedule/requestnode.h \
    schedule/requestqueuecon.h \
    rms.h \
    rmsfactory.h \
    rmsmonitor.h \
    session.h \
    singleapplication.h \
    SocketModel.h \
    sqlmodule.h \
    table.h \
    TableModel.h \
    TableView.h \
    tcpserver.h \
    tcpsocket.h \
    echo/bingquwindow.h \
    echo/fingertoolwindow.h \
    echo/miniwindow.h \
    echo/mirdebug.h \
    echo/monitor.h \
    echo/rmswindow.h \
    echo/serialdialog.h \
    echo/sysconfigWindow.h \
    echo/syswindow.h \
    echo/xiangtiwindow.h \
    echo/yihuwindow.h \
    sql/bingqutablecon.h \
    sql/boxtablecon.h \
    sql/recordtablecon.h \
    sql/sqliteutil.h \
    sql/yihutablecon.h \
    chatserver/chatserver.h \
    chatserver/serverworker.h \
    applyform/applyform.h \
    applyform/applyjson.h \
    echoWindow/boxwindow.h \
    echoWindow/boxitemdelegte.h \
    echoWindow/boxmodel.h \
    controller/boxcon.h \
    echoWindow/stockwindow.h \
    echoWindow/listmodel.h \
    echoWindow/MuItemDelegate.h \
    echoWindow/cellitem/cellitem.h \
    echoWindow/cellitem/draweritem.h \
    appconfig/sysconfig.h \
    echoWindow/chart/chartmodel.h \
    echoWindow/chart/chartitemdelegte.h \
    echoWindow/report/ButtonDelegate.h \
    echoWindow/report/reportview.h \
    echoWindow/report/reportdata.h \
    echoWindow/report/reportmodel.h \
    echoWindow/reportwindow.h \
    echoWindow/stockcontent/stockcontentwindow.h \
    controller/reportcon.h \
    excel/excelengine.h \
    excel/excelnode.h \
    controller/excelcon.h \
    itemdef.h

FORMS += \
    bingwindow.ui \
    fingertoolbox.ui \
    login.ui \
    rmsmonitor.ui \
    echo/bingquwindow.ui \
    echo/fingertoolwindow.ui \
    echo/miniwindow.ui \
    echo/mirdebug.ui \
    echo/monitor.ui \
    echo/serialdialog.ui \
    echo/sysconfigwindow.ui \
    echo/syswindow.ui \
    echo/xiangtiwindow.ui \
    echo/yihuwindow.ui \
    echoWindow/boxwindow.ui \
    echoWindow/stockwindow.ui \
    echoWindow/reportwindow.ui \
    echoWindow/stockcontent/stockcontentwindow.ui

RESOURCES += \
    resource.qrc
