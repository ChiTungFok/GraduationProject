#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "basewindow.h"
#include "chatwindow.h"
#include "groupwindow.h"
#include "global.h"
#include "addfrinedorgroup.h"
#include "roomcreate.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SetupMenu();
public slots:
    void AddFriend_Room();
    void DeleteFriend();
    void CreateRoom();
private:
    Ui::MainWindow *ui;
    QMenu *AddFriend_RoomButtom;
    QMenu *FriendMenu;
    QMenu *GroupMenu;
    QMenu *OwnerMenu;
    QXmppClient *client;
    QXmppVCardManager *VcdMng;
    QXmppRosterManager *RstMng;
    QXmppMucManager *MucMng;
    QXmppBookmarkManager *BookMng;
    QXmppTransferManager *TransferMng;
    QMap<QString, QPair<QTreeWidgetItem *, QLabel *>> Friends;
    QMap<QString, int> ChatWindowIndex;
    QMap<QString, int> GroupWindowIndex;
    QMap<QString, QTreeWidgetItem *> Bookmarks;
    QTreeWidgetItem *OperateItem_Friend;
    QTreeWidgetItem *OperateItem_Group;
    void windowClosed();
private slots:
    void on_FriendTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_GroupTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void FriendPresenceChangedHandler(const QString &bareJid, const QString &resource);
    void SubscriptionHandler(const QString &bareJid);
    void bookmarksReceivedHandler(const QXmppBookmarkSet &bookmarks);
    void MessageReceivedHandler(const QXmppMessage &msg);
    void LeftRoom();
    void DestoryRoom();
signals:
    void MessageSignal(const QXmppMessage &msg);
    void MemberList();
};

#endif // MAINWINDOW_H
