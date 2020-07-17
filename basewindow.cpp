#include "basewindow.h"

BaseWindow::BaseWindow(QWidget *parent) : QWidget(parent)
{

}

void BaseWindow::initStyle()
{
    //FramelessWindowHint属性设置窗口去除边框
    //Qt::Window是一个窗口，有窗口边框和标题
    setWindowFlags(Qt::FramelessWindowHint|Qt::Window);

    QDesktopWidget *desktop = QApplication::desktop(); //获取屏幕信息
    //局中显示窗口
    this->move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    //创建按钮组件
    QToolButton *minButton = new QToolButton(this);
    QToolButton *closeButton = new QToolButton(this);

    //获取按钮图标，这里使用默认图标
    QPixmap minPix = this->style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    QPixmap closePix = this->style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

    //设置按钮组件图标
    minButton->setIcon(minPix);
    closeButton->setIcon(closePix);

    //设置按钮位置
    minButton->setGeometry(this->width() - 50, 1, 20, 20);
    closeButton->setGeometry(this->width() - 20, 1, 20, 20);

    //设置提示信息
    minButton->setToolTip(QString("最小化"));
    closeButton->setToolTip(QString("关闭"));

    //设置按钮样式
    minButton->setStyleSheet("background:none;border:none");
    closeButton->setStyleSheet("background:none;border:none");

    //设置信号与槽函数的映射
    this->connect(closeButton, &QToolButton::clicked, this, &BaseWindow::windowClosed);
    this->connect(minButton, &QToolButton::clicked, this, &BaseWindow::windowMinimized);

}

void BaseWindow::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - lastPos.x();
    int dy = e->globalY() - lastPos.y();
    this->move(this->x() + dx, this->y() + dy);
    lastPos = e->globalPos();

}

void BaseWindow::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->globalPos();
}

void BaseWindow::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - lastPos.x();
    int dy = e->globalY() - lastPos.y();
    this->move(this->x() + dx, this->y() + dy);
}

void BaseWindow::windowClosed()
{
    this->close();
}

void BaseWindow::windowMinimized()
{
    this->showMinimized();
}
