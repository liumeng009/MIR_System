#include "rmswindouw.h"

RMSWindouw::RMSWindouw(QWidget *parent) : QMainWindow(parent)
{
    //设置主窗口标题
    setWindowTitle(tr("Mir管理系统"));

    createActions();
    createMenus();
}

void RMSWindouw::createActions()
{
    //通过图片路径来实现添加Qt资源文件目的
    socketAction = new QAction(tr("Socket"));
    socketAction->setShortcut(tr("Ctrl+O"));
    socketAction->setStatusTip(tr("socket表"));
    connect(socketAction,SIGNAL(triggered()),this,SLOT(slotOpenFile()));

    bingquAction = new QAction(tr("病区表"));
    bingquAction->setShortcut(tr("Ctrl+N"));
    bingquAction->setStatusTip(tr("病区表"));
    connect(bingquAction,SIGNAL(triggered()),this,SLOT(slotNewFile()));

    yihuAction = new QAction(tr("医护表"));
    yihuAction->setShortcut(tr("Ctrl+S"));
    yihuAction->setStatusTip(tr("医护表"));
    connect(yihuAction,SIGNAL(triggered()),this,SLOT(slotSaveFile()));

    aboutAction = new QAction(tr("About"));
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(slotAbout()));

}

void RMSWindouw::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(socketAction);
    fileMenu->addAction(bingquAction);
    fileMenu->addAction(yihuAction);
    //fileMenu->addAction(exitAction);


    editMenu = menuBar()->addMenu(tr("Edit"));

    aboutMenu = menuBar()->addMenu(tr("Help"));
    aboutMenu->addAction(aboutAction);

}

