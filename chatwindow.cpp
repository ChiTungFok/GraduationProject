#include "chatwindow.h"
#include "ui_chatwindow.h"

ChatWindow::ChatWindow(QString Jid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow),
    Receiver(Jid),
    client(GobalClient.getClient()),
    FileMng(GobalClient.getClient()->findExtension<QXmppTransferManager>())
{
    ui->setupUi(this);
    QLabel *FriendJid = new QLabel(this);
    FriendJid->setText(Jid);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(FriendJid);
    ui->frame_4->setLayout(hLayout);
    ui->content->installEventFilter(this);
    this->connect(client, &QXmppClient::messageReceived, this, &ChatWindow::MessageReceived);
    this->connect(ui->pushButton_2, &QPushButton::clicked, this, &ChatWindow::SendFile);
    this->connect(FileMng, &QXmppTransferManager::fileReceived, this, &ChatWindow::fileReceivedHandler);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}


void ChatWindow::MessageSend()
{
    start = true;
    QString Sender = client->configuration().jidBare();
    QString content = ui->content->toPlainText();
    QDateTime time = QDateTime::currentDateTime();
    QXmppMessage message(Sender, Receiver, content);
    message.setType(QXmppMessage::Chat);
    //message.setType(QXmppMessage::Type::Headline);
    message.setStamp(time);
    client->sendPacket(message);
    ui->content->clear();
    DisplaTime(QString::number(time.toTime_t()));
    //DispalyMessage(content, QString::number(time.toTime_t()), QNChatMessage::User_Sender);
    QStringList name = client->configuration().jid().split('@');
    this->feedItem = new FeedBackListItem(name[0], content, FeedBackListItem::Right, this);
    this->listItem = new QListWidgetItem(ui->listWidget);
    this->listItem->setSizeHint(QSize(this->width() - 10, 50));
    ui->listWidget->setItemWidget(this->listItem, feedItem);
    ui->listWidget->scrollToBottom();
    this->connect(this->feedItem, &FeedBackListItem::sendItemSize, this, &ChatWindow::getItemSize);
}

bool ChatWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->content && event->type() == QEvent::KeyPress) {
        int key = (static_cast<QKeyEvent *>(event))->key();

        if (Qt::Key_Return == key || Qt::Key_Enter == key) {
            MessageSend();
            return true;
        } else {
            return false;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void ChatWindow::DispalyMessage(QString msg, QString time, QNChatMessage::User_Type type)
{
    QNChatMessage *msgbubble = new QNChatMessage(ui->listWidget->parentWidget());
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    msgbubble->setFixedWidth(this->width());
    QSize size = msgbubble->fontRect(msg);
    item->setSizeHint(size);
    msgbubble->setText(msg, time, size, type);
    ui->listWidget->setItemWidget(item, msgbubble);
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
}

void ChatWindow::DisplaTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0) {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        QNChatMessage* messageTime = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->listWidget);

        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }
}

void ChatWindow::MessageReceived(const QXmppMessage &msg)
{
    //qDebug() << msg.body();
    if (QXmppUtils::jidToBareJid(msg.from()) == this->Receiver && msg.type() == QXmppMessage::Type::Chat && (!msg.body().isEmpty())) {
        FeedBackListItem::Orientation enmu = FeedBackListItem::Orientation::None;
        if (msg.from() != client->configuration().jid())
            enmu = FeedBackListItem::Orientation::Left;
        else {
            if (start == false) {
                enmu = FeedBackListItem::Orientation::Right;
            } else {
                return;
            }
        }
        QDateTime time = msg.stamp();
        QString content = msg.body();
        DisplaTime(QString::number(time.toTime_t()));
        //DispalyMessage(content, QString::number(time.toTime_t()), QNChatMessage::User_Receiver);
        QStringList from = msg.from().split('@');
        this->feedItem = new FeedBackListItem(from[0], content, enmu, this);
        this->listItem = new QListWidgetItem(ui->listWidget);
        this->listItem->setSizeHint(QSize(this->width() - 10, 50));
        ui->listWidget->setItemWidget(this->listItem, feedItem);
        ui->listWidget->scrollToBottom();
        this->connect(this->feedItem, &FeedBackListItem::sendItemSize, this, &ChatWindow::getItemSize);
    }
}

void ChatWindow::SendFile()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this, "选择文件", "");
    if (filename.isEmpty()) {
        return;
    } else {
        this->FileMng->sendFile(this->Receiver + "/QXmpp", filename);
    }
}

void ChatWindow::getItemSize(int w, int h)
{
    this->listItem->setSizeHint(QSize(w, h));
}

void ChatWindow::fileReceivedHandler(QXmppTransferJob *job)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "文件接收请求", ("用户" + Receiver + "发送文件请求， 是否接受？"),
                                                  QMessageBox::Yes | QMessageBox::No)) {
        QString filename;
        filename = QFileDialog::getExistingDirectory(this, "保存在", "");
        if (filename.isEmpty()) {
            return;
        } else {
            job->accept(filename + "/" + job->fileName());
            connect(job, &QXmppTransferJob::finished, this, [=]() {
               QMessageBox::information(this, "传输结束", "传输结束");
            });
        }

    } else {
        job->abort();
    }
}
