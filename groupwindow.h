#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QTime>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QMenu>
#include <QAction>
#include "global.h"
#include "client.h"
#include "qnchatmessage.h"
#include "feedbacklistitem.h"

namespace Ui {
class GroupWindow;
}

class GroupWindow : public QWidget
{
    Q_OBJECT

public:

    enum Mode{
        Random,
        Order,
        Specity
    };

    explicit GroupWindow(QString Jid, QXmppMucManager *MucMng, QWidget *parent = nullptr);
    ~GroupWindow();
    void MemberAddHandler(const QString &jid);
    void MemberLeaveHandler(const QString &jid);
    void MessageReceivedHandler(const QXmppMessage &msg);
    void SendMessageHandler();
    void permissionsReceivedHandler(const QList<QXmppMucItem> &permissions);
    void DispalyMessage(QString msg, QString time, QNChatMessage::User_Type type);
    void DisplaTime(QString curMsgTime);
    void SetupMenu();
    void TurnModeRandom();

public slots:
    void getItemSize(int w, int h);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::GroupWindow *ui;
    Mode status = Mode::Random;
    QString RoomJid;
    QXmppClient *client;
    QXmppMucManager *manager;
    QXmppMucRoom *room;
    QListWidgetItem *listItem;
    QListWidgetItem *Member_Item = nullptr;
    FeedBackListItem *feedItem;
    QMenu *menu;
    QMenu *MemberMenu;
    QMap<QString, QPair<QListWidgetItem *, QString>> member;
    QList<QXmppMucItem> permisson;
    QStringList AbandonedMember;
    QStringList order;
    QStringList::iterator next = nullptr;
    bool start = false;
    void Order_Next();
    void KickMember();
    void BanVoice();
    void GiveVoice();
    void SpecifyVoice();
    void subjectChangedHandler(const QString &subject);

};

#endif // GROUPWINDOW_H
