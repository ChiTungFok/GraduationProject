#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"

loginwindow::loginwindow(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::loginwindow),
    client(GobalClient.getClient()),
    width(0),
    heigth(0)
{
    ui->setupUi(this);
    this->initStyle(); //使用自定义的窗口风格

    //连接按钮信号与槽函数
    this->connect(ui->loginButton, &QPushButton::clicked ,this, &loginwindow::login);
    this->connect(ui->registButton, &QPushButton::clicked, this, &loginwindow::regist);

    QRegExp passwd_regx("^.{,16}$"); //密码长度最多为16
    QRegExpValidator *validator_pwd = new QRegExpValidator(passwd_regx, this); //生成正则匹配规则
    ui->pwdEdit->setValidator(validator_pwd); //匹配
    ui->pwdEdit->setEchoMode(QLineEdit::Password);//设置为密码样式

    this->connect(this->client, &QXmppClient::connected, this, &loginwindow::Success);

    this->connect(this->client, &QXmppClient::error, this, [=]() {
                QMessageBox::critical(this, tr("登陆失败"), client->socketErrorString());
                this->client->disconnectFromServer();
                ui->jidEdit->clear();
                ui->pwdEdit->clear();
        });
}

loginwindow::~loginwindow()
{
    delete ui;
}

void loginwindow::login()
{
    //获取用户名、密码
    QString jid = ui->jidEdit->text().trimmed();
    QString pwd = ui->pwdEdit->text();

    if (jid.isEmpty()) {
        QMessageBox::critical(this, "错误", "请输入用户名！");
        return;
    } else if (pwd.isEmpty()) {
        QMessageBox::critical(this, "错误", "请输入密码！");
        return;
    }
    this->client->addExtension(new QXmppBookmarkManager);
    this->client->addExtension(new QXmppMucManager);
    this->client->addExtension(new QXmppDiscoveryManager);
    this->client->addExtension(new QXmppTransferManager);

    int x_3 = ui->frame_3->x();
    int y_3 = ui->frame_3->y();

    QPropertyAnimation *anime1=new QPropertyAnimation(ui->frame_3,"geometry");
    anime1->setDuration(1000);
    anime1->setStartValue(QRect(ui->frame_3->x(), ui->frame_3->y(), ui->frame_3->width(), ui->frame_3->height()));
    anime1->setEndValue(QRect(ui->frame_3->x(), ui->frame->height(), ui->frame_3->width(), ui->frame_3->height()));
    //anime5->setKeyValueAt(0.0001,QRect(ui->frame_3->x(), ui->frame->height(), ui->frame_3->width(), ui->frame_3->height()));
    //anime5->setKeyValueAt(0.9,QRect(ui->frame_3->x(), ui->frame->height(), ui->frame_3->width(), ui->frame_3->height()));
    anime1->setEasingCurve(QEasingCurve::OutCubic);
    anime1->start();

    width = ui->frame_2->width();
    heigth = ui->frame_2->height();
    /*int x = ui->frame_2->x();
    int y = ui->frame_2->y();*/

    QPropertyAnimation *anime2=new QPropertyAnimation(ui->frame_2,"geometry");
    anime2->setDuration(1000);
    anime2->setStartValue(QRect(ui->frame_2->x(), ui->frame_2->y(), ui->frame_2->width(), ui->frame_2->height()));
    anime2->setEndValue(QRect(ui->frame_2->x(), ui->frame_2->y(), ui->frame_2->width(), ui->frame->height()));
    anime2->setEasingCurve(QEasingCurve::OutCubic);
    anime2->start();

    QEventLoop eventloop1;
    QTimer::singleShot(1000, &eventloop1, SLOT(quit()));
    eventloop1.exec();

    QEventLoop eventloop2;

    QPropertyAnimation *anime3=new QPropertyAnimation(ui->frame_2,"geometry");
    anime3->setDuration(2000);
    anime3->setStartValue(QRect(0, 0, ui->frame_2->width(), ui->frame->height()));
    anime3->setKeyValueAt(0.33333,QRect(-ui->frame->x(), ui->frame->y(), ui->frame->width() * 2,  ui->frame->height()));
    anime3->setKeyValueAt(0.66667,QRect(2 * ui->frame->x(), ui->frame->y(), ui->frame->width() * 2,  ui->frame->height()));
    anime3->setEndValue(QRect(0, 0, width, ui->frame->height()));
    anime3->setEasingCurve(QEasingCurve::Linear);
    anime3->setLoopCount(10);
    anime3->start();


    this->connect(this->client, &QXmppClient::connected, &eventloop2, &QEventLoop::quit);
    this->connect(this->client, &QXmppClient::error, &eventloop2, &QEventLoop::quit);
    this->connect(this->client, &QXmppClient::disconnected, &eventloop2, &QEventLoop::quit);
    QXmppConfiguration config;
    config.setHost("120.26.43.184");
    config.setPort(5222);
    config.setUser(jid);
    config.setPassword(pwd);
    config.setIgnoreSslErrors(true);
    this->client->connectToServer(config);
    eventloop2.exec();

    QPropertyAnimation *anime4=new QPropertyAnimation(ui->frame_3,"geometry");
    anime4->setDuration(2000);
    anime4->setStartValue(QRect(ui->frame_3->x(), ui->frame->height(), ui->frame_3->width(), ui->frame_3->height()));
    anime4->setEndValue(QRect(x_3, y_3, ui->frame_3->width(), ui->frame_3->height()));
    anime4->setEasingCurve(QEasingCurve::InCubic);
    anime4->start();

    QPropertyAnimation *anime5=new QPropertyAnimation(ui->frame_2,"geometry");
    anime5->setDuration(2000);
    anime5->setStartValue(QRect(0, 0, ui->frame->width(), ui->frame->height()));
    anime5->setEndValue(QRect(0, 0, width, heigth));
    anime5->setEasingCurve(QEasingCurve::InCubic);
    anime5->start();

}

void loginwindow::Success()
{
    //QMessageBox::information(this, "title", "connected");
    //is_login_success = true;
    //loop.quit();
    this->close();
    MainWindow *w = new MainWindow(this);
    w->show();
}


void loginwindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return : this->login(); break;
        case Qt::Key_Escape : this->windowClosed(); break;
    }
}

void loginwindow::regist()
{
    RegistWindow *reg = new RegistWindow(this); //必须使用动态内存，如果使用局部变量，则对象在函数结束后被析构，使得窗口只会一闪而过
    //reg->setAttribute(Qt::WA_DeleteOnClose); //子窗口指针在关闭后自动释放
    reg->show();
}

void loginwindow::windowClosed()
{
    if (this->client->isConnected()) {
        this->client->disconnectFromServer();
    }
    this->close();
}
