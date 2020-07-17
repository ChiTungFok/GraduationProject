#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QTime>
#include <QListWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include "global.h"
#include "client.h"
#include "qnchatmessage.h"
#include "feedbacklistitem.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:

    explicit ChatWindow(QString Jid, QWidget *parent = nullptr);
    ~ChatWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::ChatWindow *ui;
    QString Receiver;
    QXmppClient *client;
    QXmppTransferManager *FileMng;
    QListWidgetItem *listItem;
    FeedBackListItem *feedItem;
    void MessageSend();
    void SendFile();
    void DispalyMessage(QString msg, QString time, QNChatMessage::User_Type type);
    void DisplaTime(QString curMsgTime);
    void fileReceivedHandler(QXmppTransferJob *job);
    bool start = false;

public slots:
    void getItemSize(int w, int h);
    void MessageReceived(const QXmppMessage &msg);
};

#endif // CHATWINDOW_H
