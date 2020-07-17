#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::MainWindow),
    AddFriend_RoomButtom(new QMenu(this)),
    FriendMenu(new QMenu(this)),
    GroupMenu(new QMenu(this)),
    OwnerMenu(new QMenu(this)),
    client(GobalClient.getClient()),
    VcdMng(GobalClient.getClient()->findExtension<QXmppVCardManager>()),
    RstMng(GobalClient.getClient()->findExtension<QXmppRosterManager>()),
    MucMng(GobalClient.getClient()->findExtension<QXmppMucManager>()),
    BookMng(GobalClient.getClient()->findExtension<QXmppBookmarkManager>()),
    TransferMng(GobalClient.getClient()->findExtension<QXmppTransferManager>()),
    OperateItem_Friend(nullptr),
    OperateItem_Group(nullptr)
{
    ui->setupUi(this);
    this->initStyle();
    ui->UserJidEdit->setText(client->configuration().jidBare().split('@')[0]);
    ui->UserJidEdit->setFontPointSize(20);
    ui->UserJidEdit->document()->setDefaultTextOption(QTextOption(Qt::AlignHCenter));
    ui->FriendTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->GroupTree->setContextMenuPolicy(Qt::CustomContextMenu);
    SetupMenu();
    this->connect(ui->FriendTree, &QListWidget::customContextMenuRequested, [this](QPoint pos){
            QTreeWidgetItem* item = ui->FriendTree->itemAt(pos);
            if (item) {
                this->OperateItem_Friend = item;
                this->FriendMenu->popup(ui->FriendTree->viewport()->mapToGlobal(pos));
            }

      });
    this->connect(ui->GroupTree, &QListWidget::customContextMenuRequested, [this](QPoint pos) {
        QTreeWidgetItem* item = ui->GroupTree->itemAt(pos);
        if (item) {
            this->OperateItem_Group = item;

            QWidget *widget = ui->GroupTree->itemWidget(this->OperateItem_Group, 0);
            QList<QLabel *> list = widget->findChildren<QLabel *>();
            QString Jid = list[0]->text();

            QList<QXmppMucRoom *> rooms = this->MucMng->rooms();

            foreach(QXmppMucRoom *room, rooms) {
                qDebug() << room->jid();
                if (room->jid() == Jid) {
                    qDebug() << Jid + '/' + this->client->configuration().user();
                    if (room->participantPresence(Jid + '/' + this->client->configuration().user()).mucItem().affiliation() == QXmppMucItem::OwnerAffiliation) {
                        this->OwnerMenu->popup(ui->GroupTree->viewport()->mapToGlobal(pos));
                        return;
                    }

                }
            }

            this->GroupMenu->popup(ui->GroupTree->viewport()->mapToGlobal(pos));
        }
    });
    ui->FriendTree->header()->hide(); //隐藏列
    ui->GroupTree->header()->hide();
    client->logger()->setLoggingType(QXmppLogger::StdoutLogging);
    ui->stackedWidget->setCurrentIndex(1);
    this->connect(RstMng, &QXmppRosterManager::subscriptionReceived, this, &MainWindow::SubscriptionHandler);
    //this->connect(ui->AddFriend_Room, &QPushButton::clicked, this, &MainWindow::AddFriend);
    this->connect(ui->FriendButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(1);
    });
    this->connect(this->client, &QXmppClient::messageReceived, this, &MainWindow::MessageReceivedHandler);

    this->connect(RstMng, &QXmppRosterManager::rosterReceived, this, [=]() {
        QStringList list;
        list = RstMng->getRosterBareJids();
        foreach(QString v, list) {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->FriendTree);
            ui->FriendTree->addTopLevelItem(item);
            QWidget *fri = new QWidget(ui->FriendTree); //设置widget，使其在QTreeWidget显示
            QHBoxLayout *hLayout=new QHBoxLayout;
            QLabel *status = new QLabel(fri);
            QLabel *jid = new QLabel(fri);
            jid->setText(v);
            status->setText("Offline"); // 一开始默认用户状态为离线
            hLayout->addWidget(jid);
            hLayout->addStretch();
            hLayout->addWidget(status);
            fri->setLayout(hLayout);
            ui->FriendTree->setItemWidget(item, 0, fri);
            this->Friends.insert(v, qMakePair(item, status));

        }
    });

    this->connect(BookMng, &QXmppBookmarkManager::bookmarksReceived, this, &MainWindow::bookmarksReceivedHandler);
    this->connect(RstMng, &QXmppRosterManager::presenceChanged, this, &MainWindow::FriendPresenceChangedHandler);
}

void MainWindow::windowClosed()
{
    if (this->client->isConnected()) {
        this->client->disconnectFromServer();
    }
    this->close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupMenu()
{
    QAction *addFriend_Room=new QAction("添加好友/群聊",this);
    QAction *createRoom=new QAction("创建群组",this);
    AddFriend_RoomButtom->addAction(addFriend_Room);
    AddFriend_RoomButtom->addAction(createRoom);
    ui->AddFriend_Room->setMenu(AddFriend_RoomButtom);
    connect(addFriend_Room, &QAction::triggered, this, &MainWindow::AddFriend_Room);
    connect(createRoom, &QAction::triggered, this, &MainWindow::CreateRoom);


    QAction *DeleteFriendAction = new QAction("删除好友", this);
    this->FriendMenu->addAction(DeleteFriendAction);
    connect(DeleteFriendAction, &QAction::triggered, this, &MainWindow::DeleteFriend);


    QAction *ExitGroup = new QAction("退出群聊", this);
    this->GroupMenu->addAction(ExitGroup);
    connect(ExitGroup, &QAction::triggered, this, &MainWindow::LeftRoom);

    this->OwnerMenu->addAction(ExitGroup);

    QAction *Destory = new QAction("解散群聊", this);
    this->OwnerMenu->addAction(Destory);
    connect(Destory, &QAction::triggered, this, &MainWindow::DestoryRoom);

}

void MainWindow::on_FriendTree_itemDoubleClicked(QTreeWidgetItem *item, int column) //双击就会连接的槽函数
{
    QWidget *widget = ui->FriendTree->itemWidget(item, column);
    QList<QLabel *> list = widget->findChildren<QLabel *>();
    QString Jid = list[1]->text();
    auto i = this->ChatWindowIndex.find(Jid);
    if (i == this->ChatWindowIndex.end()) {
        ChatWindow *chat = new ChatWindow(Jid, this);
        int index = ui->stackedWidget_2->addWidget(chat);
        ui->stackedWidget_2->setCurrentIndex(index);
        ChatWindowIndex.insert(Jid, index);
    } else {
        ui->stackedWidget_2->setCurrentIndex(i.value());
    }
}

void MainWindow::on_GroupTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QWidget *widget = ui->GroupTree->itemWidget(item, column);
    QList<QLabel *> list = widget->findChildren<QLabel *>();
    QString Jid = list[0]->text();
    auto i = GroupWindowIndex.find(Jid);
    if (i == GroupWindowIndex.end()) {
        GroupWindow *group = new GroupWindow(Jid, MucMng, this);
        int index = ui->stackedWidget_2->addWidget(group);
        ui->stackedWidget_2->setCurrentIndex(index);
        GroupWindowIndex.insert(Jid, index);
    } else {
        ui->stackedWidget_2->setCurrentIndex(i.value());
    }
}

void MainWindow::FriendPresenceChangedHandler(const QString &bareJid, const QString &resource)
{
    QXmppPresence presence = RstMng->getPresence(bareJid, resource);
    QLabel *status = nullptr;
    if (bareJid == client->configuration().jidBare()) return;
    auto index = this->Friends.find(bareJid);
    if (index == Friends.end()) {
        return;
    } else {
        switch (presence.type()) {
            case QXmppPresence::Type::Error:
                status = index.value().second;
                status->setText("Error");
                break;
            case QXmppPresence::Type::Available:
                status = index.value().second;
                if (presence.statusText().isEmpty())
                    status->setText("Online");
                else
                    status->setText(presence.statusText());
                break;
            case QXmppPresence::Type::Unavailable:
                status = index.value().second;
                status->setText("Offline");
                break;
            default: break;
        }
    }
}

void MainWindow::SubscriptionHandler(const QString &bareJid)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "添加好友申请", ("用户" + bareJid + "申请与您成为好友， 是否接受？"),
                                                  QMessageBox::Yes | QMessageBox::No)) {
        RstMng->acceptSubscription(bareJid);
        RstMng->subscribe(bareJid);

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->FriendTree);
        ui->FriendTree->addTopLevelItem(item);
        QWidget *fri = new QWidget(ui->FriendTree); //设置widget，使其在QTreeWidget显示
        QHBoxLayout *hLayout=new QHBoxLayout;
        QLabel *status = new QLabel(fri);
        QLabel *jid = new QLabel(fri);
        jid->setText(bareJid);
        status->setText("Offline"); // 一开始默认用户状态为离线
        QStringList resourcesList = RstMng->getResources(bareJid);
        foreach(QString resources, resourcesList) {
            QXmppPresence presence = RstMng->getPresence(bareJid, resources);
            if (presence.type() == QXmppPresence::Type::Available) {
                status->setText("Online");
                break;
            }
        }
        hLayout->addWidget(jid);
        hLayout->addStretch();
        hLayout->addWidget(status);
        fri->setLayout(hLayout);
        ui->FriendTree->setItemWidget(item, 0, fri);
        this->Friends.insert(bareJid, qMakePair(item, status));


    } else {
        RstMng->refuseSubscription(bareJid);
    }
}

void MainWindow::bookmarksReceivedHandler(const QXmppBookmarkSet &bookmarks)
{
    QList<QXmppBookmarkConference> conferenceList = bookmarks.conferences();
    foreach(QXmppBookmarkConference conference, conferenceList) {
        auto i = this->Bookmarks.find(conference.jid());
        if (i == this->Bookmarks.end()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->GroupTree);
            ui->GroupTree->addTopLevelItem(item);
            QWidget *Grp = new QWidget(ui->GroupTree);
            QHBoxLayout *hLayout=new QHBoxLayout;
            QLabel *GrpJid = new QLabel(Grp);
            GrpJid->setText(conference.jid());
            hLayout->addWidget(GrpJid);
            Grp->setLayout(hLayout);
            ui->GroupTree->setItemWidget(item, 0, Grp);
            this->Bookmarks.insert(conference.jid(), item);

            GroupWindow *group = new GroupWindow(conference.jid(), MucMng, this);
            int index = ui->stackedWidget_2->addWidget(group);
            //ui->stackedWidget_2->setCurrentIndex(index);
            GroupWindowIndex.insert(conference.jid(), index);


        }
    }
}

void MainWindow::AddFriend_Room()
{
    auto i = new AddFrinedorGroup(this);
    i->show();
}

void MainWindow::CreateRoom()
{
    auto i = new RoomCreate(this);
    i->show();
}

void MainWindow::DeleteFriend()
{
    if (this->OperateItem_Friend == nullptr) return;
    QWidget *widget = ui->FriendTree->itemWidget(this->OperateItem_Friend, 0);
    QList<QLabel *> list = widget->findChildren<QLabel *>();
    QString bareJid = list[1]->text();

    RstMng->unsubscribe(bareJid);
    RstMng->removeItem(bareJid);

    ui->FriendTree->removeItemWidget(this->OperateItem_Friend, 0);
    delete this->OperateItem_Friend;
    this->OperateItem_Friend = nullptr;
    auto i = this->Friends.find(bareJid);
    this->Friends.erase(i);

    auto j = this->ChatWindowIndex.find(bareJid);
    ChatWindow *chat =  (ChatWindow *)ui->stackedWidget_2->widget(j.value());
    delete chat;
    chat = nullptr;
    ChatWindowIndex.erase(j);

}

void MainWindow::LeftRoom()
{
    if (this->OperateItem_Group == nullptr) return;
    QWidget *widget = ui->GroupTree->itemWidget(this->OperateItem_Group, 0);
    QList<QLabel *> list = widget->findChildren<QLabel *>();
    QString Jid = list[0]->text();

    QList<QXmppMucRoom *> rooms = MucMng->rooms();
    foreach(QXmppMucRoom *room, rooms) {
        if (room->jid() == Jid) {
            room->leave();
            break;
        } else continue;
    }

    QXmppBookmarkSet set = BookMng->bookmarks();
    QList<QXmppBookmarkConference> conferenceList = set.conferences();
    for (auto i = conferenceList.begin(); i != conferenceList.end(); ++i) {
        if (i->jid() == Jid) {
            conferenceList.erase(i);
            break;
        } else continue;
    }
    set.setConferences(conferenceList);
    BookMng->setBookmarks(set);

    ui->GroupTree->removeItemWidget(this->OperateItem_Group, 0);
    delete this->OperateItem_Group;
    this->OperateItem_Group = nullptr;
    auto i = this->Bookmarks.find(Jid);
    this->Bookmarks.erase(i);

    auto j = this->GroupWindowIndex.find(Jid);
    GroupWindow *grp = (GroupWindow *)ui->stackedWidget_2->widget(j.value());
    delete grp;
    grp = nullptr;
    GroupWindowIndex.erase(j);

}

void MainWindow::DestoryRoom()
{
    if (this->OperateItem_Group == nullptr) return;
    QWidget *widget = ui->GroupTree->itemWidget(this->OperateItem_Group, 0);
    QList<QLabel *> list = widget->findChildren<QLabel *>();
    QString Jid = list[0]->text();

    QXmppElementList membersEls;
    QXmppElement queryEl;
    QXmppElement destoryEl;


    destoryEl.setTagName("destroy");

    queryEl.setTagName("query");
    queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#owner");
    queryEl.appendChild(destoryEl);

    membersEls.append(queryEl);
    QXmppIq Iq(QXmppIq::Set);
    Iq.setTo(Jid);
    Iq.setFrom(client->configuration().jidBare());
    Iq.setExtensions(membersEls);
    client->sendPacket(Iq);

    ui->GroupTree->removeItemWidget(this->OperateItem_Group, 0);
    delete this->OperateItem_Group;
    this->OperateItem_Group = nullptr;
    auto i = this->Bookmarks.find(Jid);
    this->Bookmarks.erase(i);

    auto j = this->GroupWindowIndex.find(Jid);
    GroupWindow *grp = (GroupWindow *)ui->stackedWidget_2->widget(j.value());
    delete grp;
    grp = nullptr;
    GroupWindowIndex.erase(j);

}

void MainWindow::MessageReceivedHandler(const QXmppMessage &msg)
{
    QStringList from = msg.from().split('/');
    if (msg.type() == QXmppMessage::Type::Chat) {

        auto index = this->ChatWindowIndex.find(from[0]);
        if (index == ChatWindowIndex.end()) {
            ChatWindow *chat = new ChatWindow(from[0], this);
            this->connect(this, &MainWindow::MessageSignal, chat, &ChatWindow::MessageReceived);
            int index = ui->stackedWidget_2->addWidget(chat);
            ui->stackedWidget_2->setCurrentIndex(index);
            ChatWindowIndex.insert(from[0], index);
            emit this->MessageSignal(msg);
        }
    }
}
