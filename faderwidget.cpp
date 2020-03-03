#include "faderwidget.h" 

#include <QPalette>
#include <QPixmap>
#include <QCursor>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QIcon>

FaderWidget::FaderWidget(QWidget *parent) :
    QWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow);

    isEnter = false;

    titleW = new TitleWidget(this);

    connect(titleW,SIGNAL(myClose()),this,SLOT(close()));



    content = new QLabel(this);

    content->setWordWrap(true);

    content->setAlignment(Qt::AlignTop);

    content->setFixedSize(300,100);

    //btnLook=new QPushButton("查看");

    //connect(btnLook,SIGNAL(clicked()),this,SLOT(seeInfo()));



    QVBoxLayout * mainLayout=new QVBoxLayout(this);

    mainLayout->setMargin(0);

    mainLayout->addWidget(titleW);

    mainLayout->addWidget(content);

    content->setMargin(5);

    //mainLayout->addWidget(btnLook,0,Qt::AlignRight);

    setLayout(mainLayout);



    setFixedSize(sizeHint().width(),sizeHint().height());



    timerShow = new QTimer(this);

    connect(timerShow,SIGNAL(timeout()),this,SLOT(myMove()));

    timerStay = new QTimer(this);

    connect(timerStay,SIGNAL(timeout()),this,SLOT(myStay()));

    timerClose = new QTimer(this);

    connect(timerClose,SIGNAL(timeout()),this,SLOT(myClose()));

}



FaderWidget::~FaderWidget()
{
}

void FaderWidget::setMsg(QString title, QString content, QString work)
{
    titleW->setTitleText("  "+title);
    this->content->setText("   "+content);
}



void FaderWidget::paintEvent(QPaintEvent *)

{

    QBitmap bitmap(this->size());

    bitmap.fill(Qt::white);

    QPainter painter(this);

    painter.setBrush(QBrush(QColor(250,240,230)));

    painter.setPen(QPen(QBrush(QColor(255,222,173)),4));

    painter.drawRoundedRect(bitmap.rect(),5,5);

    setMask(bitmap);

}

void FaderWidget::showAsQQ()

{

    QDesktopWidget *deskTop=QApplication::desktop();

    deskRect = deskTop->availableGeometry();



    normalPoint.setX(deskRect.width()-rect().width()-1);

    normalPoint.setY(deskRect.height()-rect().height());

    move(normalPoint.x(),deskRect.height()-1);

    show();

    timerShow->start(1);
}

void FaderWidget::InfoPrint(QString title, QString content)
{
    setMsg(title, content, QStringLiteral(""));
    showAsQQ();
}

//平滑显示出来

void FaderWidget::myMove()

{

    static int beginY=QApplication::desktop()->height();

    beginY--;

    move(normalPoint.x(),beginY);

    if(beginY<=normalPoint.y())

    {

        timerShow->stop();

        timerStay->start(1000);

    }

}

//停留显示

void FaderWidget::myStay()

{

    static int timeCount=0;

    timeCount++;

    if(timeCount>=5)

    {

        timerStay->stop();

        timerClose->start(200);

        tran = 1.0;

    }

}

//自动关闭时实现淡出效果
void FaderWidget::myClose()
{
    if(isEnter){

        tran = 1.0;

        setWindowOpacity(tran);

        return;

    }

    tran-=0.1;

    if(tran<=0.0)

    {

        timerClose->stop();

        //emit close();
        //hide();
        deleteLater();
    }

    else

        setWindowOpacity(tran);

}

void FaderWidget::seeInfo()

{


}



void FaderWidget::enterEvent(QEvent *)

{

    isEnter = true;

}

void FaderWidget::leaveEvent(QEvent *)

{

    isEnter = false;

}


TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent)
{
    titleText=new QLabel;
    btnClose = new QToolButton(this);
    btnClose->setObjectName(QString::fromUtf8("btnClose"));
    btnClose->setToolTip(tr("关闭"));
    btnClose->setStyleSheet(QString::fromUtf8("QWidget[objectName=\"btnClose\"]{\n"
        "border-width: 0px;\n"
        "border-style: solid;\n"
        "padding: 0px;\n"
        "padding-left: 0px;\n"
        "padding-right: 0px;\n"
        "min-width: 16px;\n"
        "max-width: 16px;\n"
        "min-height: 16px;\n"
        "max-height: 16px;\n"
        "background-image: url(:/Resources/btn_close_normal.bmp);\n"
        "}\n"
        "\n"
        "QWidget[objectName=\"btnClose\"]:hover{\n"
        "background-image: url(:/Resources/btn_close_highlight.bmp);\n"
        "}\n"
        "\n"
        "QWidget[objectName=\"btnClose\"]:pressed{\n"
        "background-image: url(:/Resources/btn_close_down.bmp);\n"
        "}\n"
        ""));
    connect(btnClose,SIGNAL(clicked()),this,SIGNAL(myClose()));
    QHBoxLayout *layout=new QHBoxLayout;
    layout->addWidget(titleText,0,Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(btnClose,0,Qt::AlignRight);
    layout->setMargin(0);
    setLayout(layout);
    setFixedHeight(20);
}
TitleWidget::~TitleWidget()
{

}
void TitleWidget::paintEvent(QPaintEvent *)
{
    QLinearGradient linear(rect().topLeft(),rect().bottomRight());
    linear.setColorAt(0,QColor(227,207,87));
    linear.setColorAt(0.5,QColor(245,222,179));
    linear.setColorAt(1,QColor(189,252,201));

    QPainter painter(this);
    painter.setBrush(QBrush(linear));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}

void TitleWidget::setTitleText(QString title)
{
    titleText->setText(title);
}

