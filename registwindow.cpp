#include "registwindow.h"
#include "ui_registwindow.h"
#include <iostream>

RegistWindow::RegistWindow(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::RegistWindow),
    Regist_Client(new QXmppClient(this))
{
    ui->setupUi(this);
    this->initStyle();

    QRegExp email_regx("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
    QRegExp passwd_regx("^.{,16}$"); //密码长度最多为16
    QRegExpValidator *validator_pwd = new QRegExpValidator(passwd_regx, this); //生成正则匹配规则
    QRegExpValidator *validator_email = new QRegExpValidator(email_regx, this);
    ui->PwdLnEdit->setValidator(validator_pwd); //匹配
    ui->PwdLnEdit->setEchoMode(QLineEdit::Password);//设置为密码样式
    ui->ConfirmLnEdit->setValidator(validator_pwd);
    ui->ConfirmLnEdit->setEchoMode(QLineEdit::Password);
    ui->EmailLnEdit->setValidator(validator_email);

    this->connect(ui->Reset, &QPushButton::clicked, this, &RegistWindow::Reset);
    this->connect(ui->Regist, &QPushButton::clicked, this, &RegistWindow::Regist);
    this->connect(this->Regist_Client, &QXmppClient::iqReceived, this, &RegistWindow::iqReceived);

    QXmppConfiguration config;
    config.setHost("120.26.43.184");
    config.setIgnoreSslErrors(true);
    config.setUseSASLAuthentication(false);
    Regist_Client->connectToServer(config);

}

RegistWindow::~RegistWindow()
{
    delete ui;
}

void RegistWindow::Regist()
{
    QString username = ui->UserLnEdit->text().trimmed();
    QString email = ui->EmailLnEdit->text().trimmed();
    QString pwd = ui->PwdLnEdit->text();
    QString confirm_pwd = ui->ConfirmLnEdit->text();

    if (username.isEmpty()) {
        QMessageBox::critical(this, "注册信息错误", "请输入用户名！");
        return;
    } else if (pwd.isEmpty()) {
        QMessageBox::critical(this, "注册信息错误", "请输入密码！");
        return;
    } else if (confirm_pwd.isEmpty()) {
        QMessageBox::critical(this, "注册信息错误", "请确认密码！");
        return;
    } else if (pwd != confirm_pwd) {
        QMessageBox::critical(this, "密码不一致", "密码不一致，请重新输入！");
        return;
    }

    QXmppRegisterIq iq;
    iq.setTo("120.26.43.184");
    iq.setType(QXmppIq::Set);
    iq.setUsername(username);
    iq.setPassword(pwd);
    if (!email.isEmpty()) iq.setEmail(email);
    Regist_Client->sendPacket(iq);
}

void RegistWindow::Reset()
{
    ui->UserLnEdit->clear();
    ui->EmailLnEdit->clear();
    ui->PwdLnEdit->clear();
    ui->ConfirmLnEdit->clear();
}

void RegistWindow::iqReceived(const QXmppIq &iq)
{
    std::cout << iq.error().type() << std::endl;

    if (iq.type() == QXmppIq::Result) {

        this->Regist_Client->disconnectFromServer();
        QMessageBox::information(this, "注册成功", "注册成功！");
        this->close();

    } else if (iq.type() == QXmppIq::Error /*&& iq.error().type() == QXmppIq::Error::Modify*/) {

        QMessageBox::critical(this, "注册失败", "注册失败！");

    }
}

void RegistWindow::closeEvent(QCloseEvent *event)
{
    if (this->Regist_Client->isConnected()) {
        this->Regist_Client->disconnectFromServer();
    }
    QWidget::closeEvent(event);
}
