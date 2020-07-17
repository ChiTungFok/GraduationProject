#include "groupwindow.h"
#include "ui_groupwindow.h"

GroupWindow::GroupWindow(QString Jid, QXmppMucManager *MucMng, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupWindow),
    RoomJid(Jid),
    client(GobalClient.getClient()),
    manager(MucMng),
    menu(new QMenu(this)),
    MemberMenu(new QMenu(this))

{
    ui->setupUi(this);
    ui->pushButton_4->hide();
    ui->textEdit->hide();
    ui->memberList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->adminList->setContextMenuPolicy(Qt::CustomContextMenu);

    QStringList nickname = client->configuration().jid().split("@");
    room = MucMng->addRoom(RoomJid);
    room->setNickName(nickname[0]);
    room->join();
    QLabel *GroupJid = new QLabel(this);
    GroupJid->setText(RoomJid);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(GroupJid);
    ui->frame_4->setLayout(hLayout);
    ui->content->installEventFilter(this);
    this->room->requestPermissions();
    this->connect(room, &QXmppMucRoom::permissionsReceived, this, &GroupWindow::permissionsReceivedHandler);
    this->connect(room, &QXmppMucRoom::participantAdded, this, &GroupWindow::MemberAddHandler);
    this->connect(room, &QXmppMucRoom::participantRemoved, this, &GroupWindow::MemberLeaveHandler);
    this->connect(room, &QXmppMucRoom::messageReceived, this, &GroupWindow::MessageReceivedHandler);
    this->connect(room, &QXmppMucRoom::permissionsReceived, this, &GroupWindow::permissionsReceivedHandler);
    this->connect(room, &QXmppMucRoom::subjectChanged, this, &GroupWindow::subjectChangedHandler);
    this->connect(ui->pushButton_4, &QPushButton::clicked, this, &GroupWindow::Order_Next);
    this->connect(ui->invitation, &QPushButton::clicked, [=]() {
        QCheckBox *checkbox = new QCheckBox("test", this);
        checkbox->show();
    });
    this->SetupMenu();
}

GroupWindow::~GroupWindow()
{
    delete ui;
}

void GroupWindow::permissionsReceivedHandler(const QList<QXmppMucItem> &permissions)
{
    this->permisson = permissions;
    foreach(QXmppMucItem item, permissions) {
        qDebug() << item.jid();
        qDebug() << item.nick();
        qDebug() << QXmppMucItem::roleToString(item.role());
    }

}

bool GroupWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->content && event->type() == QEvent::KeyPress) {
        int key = (static_cast<QKeyEvent *>(event))->key();

        if (Qt::Key_Return == key || Qt::Key_Enter == key) {
            this->SendMessageHandler();
            return true;
        } else {
            return false;
        }
    }

    return QWidget::eventFilter(watched, event);
}


void GroupWindow::Order_Next()
{
    QStringList::iterator b = this->next;
    this->next++;
    QXmppElementList membersEls;
    QXmppElement queryEl;
    QXmppElement itemEl;

    itemEl.setTagName("item");
    itemEl.setAttribute("nick", b->split('/')[1]);
    itemEl.setAttribute("role", "visitor");

    queryEl.setTagName("query");
    queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
    queryEl.appendChild(itemEl);

    membersEls.append(queryEl);

    QXmppIq Iq(QXmppIq::Set);
    Iq.setTo(room->jid());
    Iq.setFrom(client->configuration().jidBare());
    Iq.setExtensions(membersEls);
    client->sendPacket(Iq);

    if (this->next == this->order.end()) {
        //this->next = nullptr;
        this->order.clear();
        this->room->setSubject("Mode::Random");
        ui->pushButton_4->hide();
    } else {
        QXmppElementList membersEls;
        QXmppElement queryEl;
        QXmppElement itemEl;

        itemEl.setTagName("item");
        itemEl.setAttribute("nick", this->next->split('/')[1]);
        itemEl.setAttribute("role", "participant");

        queryEl.setTagName("query");
        queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
        queryEl.appendChild(itemEl);

        membersEls.append(queryEl);

        QXmppIq Iq(QXmppIq::Set);
        Iq.setTo(room->jid());
        Iq.setFrom(client->configuration().jidBare());
        Iq.setExtensions(membersEls);
        client->sendPacket(Iq);
        this->room->setSubject(this->next->split('/')[1]);
    }
}

void GroupWindow::SetupMenu()
{
    QAction *random = new QAction("随机发言", this);
    QAction *order = new QAction("顺序发言",this);
    //QAction *specify = new QAction("指定发言",this);

    this->menu->addAction(random);
    this->menu->addAction(order);
    //this->menu->addAction(specify);
    ui->pushButton_3->setMenu(menu);
    this->connect(random, &QAction::triggered, this, &GroupWindow::TurnModeRandom);

    this->connect(order, &QAction::triggered, this, [=]() {
        this->status = GroupWindow::Mode::Order;
        this->room->setSubject("Mode::Order");
        ui->pushButton_4->show();
        //this->room->setSubject(this->client->configuration().jidBare());

        for (auto i = this->member.begin(); i != this->member.end(); ++i) {
            if (this->room->participantPresence(i.key()).mucItem().role() == QXmppMucItem::Role::ParticipantRole) {
                this->order.append(i.key());
                QXmppElementList membersEls;
                QXmppElement queryEl;
                QXmppElement itemEl;

                itemEl.setTagName("item");
                itemEl.setAttribute("nick", i.key().split('/')[1]);
                itemEl.setAttribute("role", "visitor");

                queryEl.setTagName("query");
                queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
                queryEl.appendChild(itemEl);

                membersEls.append(queryEl);

                QXmppIq Iq(QXmppIq::Set);
                Iq.setTo(room->jid());
                Iq.setFrom(client->configuration().jidBare());
                Iq.setExtensions(membersEls);
                client->sendPacket(Iq);

            }
        }
        this->next = this->order.begin();

        if (this->next != this->order.end()) {
            QXmppElementList membersEls;
            QXmppElement queryEl;
            QXmppElement itemEl;

            itemEl.setTagName("item");
            itemEl.setAttribute("nick", this->next->split('/')[1]);
            itemEl.setAttribute("role", "participant");

            queryEl.setTagName("query");
            queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
            queryEl.appendChild(itemEl);

            membersEls.append(queryEl);

            QXmppIq Iq(QXmppIq::Set);
            Iq.setTo(room->jid());
            Iq.setFrom(client->configuration().jidBare());
            Iq.setExtensions(membersEls);
            client->sendPacket(Iq);
        }

        this->room->setSubject(this->next->split('/')[1]);

        //this->room->requestPermissions();
    });
}

void GroupWindow::MemberAddHandler(const QString &jid)
{
    qDebug() << jid;
    qDebug() << QXmppMucItem::roleToString(room->participantPresence(jid).mucItem().role());
    qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(jid).mucItem().affiliation());

    if (room->participantPresence(jid).mucItem().affiliation() == QXmppMucItem::Affiliation::NoAffiliation) {
        if (room->participantPresence(RoomJid + "/" + room->nickName()).mucItem().affiliation()
                == QXmppMucItem::AdminAffiliation || room->participantPresence(RoomJid + "/" + room->nickName()).mucItem().affiliation()
                == QXmppMucItem::OwnerAffiliation) {
            QList<QXmppMucItem> p;
            QXmppMucItem item;
            item.setAffiliation(QXmppMucItem::MemberAffiliation);
            item.setJid(room->participantFullJid(jid).split('/')[0]);
            p.append(item);
            room->setPermissions(p);
        }
    }

    QStringList nick = jid.split('/');
    //qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(jid).mucItem().affiliation());
    switch (room->participantPresence(jid).mucItem().affiliation()) {
        case QXmppMucItem::Affiliation::OwnerAffiliation : {
            QListWidgetItem *item = new QListWidgetItem(ui->ownerList);
            item->setText(nick[1]);
            ui->ownerList->addItem(item);
            this->member.insert(jid, qMakePair(item, room->participantPresence(jid).mucItem().affiliation()));

            if (jid == RoomJid + "/" + room->nickName()) {
                QAction *banAction = new QAction("踢出房间", this);
                QAction *banVoice = new QAction("禁言", this);
                QAction *getVoice = new QAction("取消禁言", this);
                QAction *Specify = new QAction("指定发言", this);
                MemberMenu->addAction(banAction);
                MemberMenu->addAction(banVoice);
                MemberMenu->addAction(getVoice);
                MemberMenu->addAction(Specify);

                this->connect(banAction, &QAction::triggered, this, &GroupWindow::KickMember);
                this->connect(banVoice, &QAction::triggered, this, &GroupWindow::BanVoice);
                this->connect(getVoice, &QAction::triggered, this, &GroupWindow::GiveVoice);
                this->connect(Specify, &QAction::triggered, this, &GroupWindow::SpecifyVoice);

                this->connect(ui->memberList, &QListWidget::customContextMenuRequested, [this](QPoint pos) {
                    QListWidgetItem *item = ui->memberList->itemAt(pos);
                    if (item) {
                        Member_Item = item;
                        MemberMenu->popup(ui->memberList->mapToGlobal(pos));
                    }
                });

            }

            break;
        }
        case QXmppMucItem::Affiliation::AdminAffiliation : {
            QListWidgetItem *item = new QListWidgetItem(ui->adminList);
            item->setText(nick[1]);
            ui->adminList->addItem(item);
            this->member.insert(jid, qMakePair(item, room->participantPresence(jid).mucItem().affiliation()));

            if (jid == RoomJid + "/" + room->nickName()) {
                QAction *banAction = new QAction("踢出房间", this);
                QAction *banVoice = new QAction("禁言", this);
                QAction *getVoice = new QAction("取消禁言", this);
                QAction *Specify = new QAction("指定发言", this);
                MemberMenu->addAction(banAction);
                MemberMenu->addAction(banVoice);
                MemberMenu->addAction(getVoice);
                MemberMenu->addAction(Specify);

                this->connect(banAction, &QAction::triggered, this, &GroupWindow::KickMember);
                this->connect(banVoice, &QAction::triggered, this, &GroupWindow::BanVoice);
                this->connect(getVoice, &QAction::triggered, this, &GroupWindow::GiveVoice);
                this->connect(Specify, &QAction::triggered, this, &GroupWindow::SpecifyVoice);

                this->connect(ui->memberList, &QListWidget::customContextMenuRequested, [this](QPoint pos) {
                    QListWidgetItem *item = ui->memberList->itemAt(pos);
                    if (item) {
                        Member_Item = item;
                        MemberMenu->popup(ui->memberList->mapToGlobal(pos));
                    }
                });
            }

            break;
        }
        case QXmppMucItem::Affiliation::MemberAffiliation :
        case QXmppMucItem::Affiliation::NoAffiliation : {
            QListWidgetItem *item = new QListWidgetItem(ui->memberList);
            item->setText(nick[1]);
            ui->memberList->addItem(item);
            this->member.insert(jid, qMakePair(item, room->participantPresence(jid).mucItem().affiliation()));
            if (jid == RoomJid + "/" + room->nickName()) ui->pushButton_3->hide();

            if (room->participantPresence(RoomJid + "/" + room->nickName()).mucItem().role() == QXmppMucItem::Role::ModeratorRole) {
                if (this->status == Mode::Specity) {
                    this->Member_Item = item;
                    BanVoice();
                } else if (this->status == Mode::Order) {
                    this->order.append(jid);
                }
            }


            //qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(jid).mucItem().affiliation());
            break;
        }
        default : break;
    }
}

void GroupWindow::MemberLeaveHandler(const QString &jid)
{
    qDebug() << jid;
    qDebug() << QXmppMucItem::roleToString(room->participantPresence(jid).mucItem().role());
    qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(jid).mucItem().affiliation());
    auto index = this->member.find(jid);
    if (index == member.end()) return;
    switch (room->participantPresence(jid).mucItem().affiliation()) {
        case QXmppMucItem::Affiliation::OwnerAffiliation : {
            ui->ownerList->removeItemWidget(index.value().first);
            delete index.value().first;
            member.erase(index);
            qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(jid).mucItem().affiliation());
            break;
        }
        case QXmppMucItem::Affiliation::AdminAffiliation : {
            ui->adminList->removeItemWidget(index.value().first);
            delete index.value().first;
            member.erase(index);
            qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(jid).mucItem().affiliation());
            break;
        }
        case QXmppMucItem::Affiliation::MemberAffiliation :
        case QXmppMucItem::Affiliation::NoAffiliation : {
            ui->memberList->removeItemWidget(index.value().first);
            delete index.value().first;
            member.erase(index);
            if ((this->next != this->order.end()) && this->status == Mode::Order) {
                this->order.removeOne(jid);
            }
            qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(jid).mucItem().affiliation());
            break;
        }
        default : break;
    }
}

void GroupWindow::SendMessageHandler()
{
    start = true;
    QString content = ui->content->toPlainText();
    QDateTime time = QDateTime::currentDateTime();
    room->sendMessage(content);
    ui->content->clear();
    DisplaTime(QString::number(time.toTime_t()));
    //QStringList from = room->nickName().split("/");
    //QString name = from[1].isNull() ? from[0] : from[1];
    this->feedItem = new FeedBackListItem(room->nickName(), content, FeedBackListItem::Right, this);
    this->listItem = new QListWidgetItem(ui->listWidget);
    this->listItem->setSizeHint(QSize(this->width() - 10, 60));
    ui->listWidget->setItemWidget(this->listItem, feedItem);
    ui->listWidget->scrollToBottom();
    this->connect(this->feedItem, &FeedBackListItem::sendItemSize, this, &GroupWindow::getItemSize);

}

void GroupWindow::MessageReceivedHandler(const QXmppMessage &msg)
{
    qDebug() << QXmppMucItem::affiliationToString(room->participantPresence(msg.from()).mucItem().affiliation());
    qDebug() << QXmppMucItem::roleToString(room->participantPresence(msg.from()).mucItem().role());
    //qDebug() << client->configuration().jid();
    qDebug() << msg.body();
    if (msg.type() == QXmppMessage::Type::GroupChat && (!msg.body().isEmpty())) {
        FeedBackListItem::Orientation enmu = FeedBackListItem::Orientation::None;
        if ((room->participantFullJid(msg.from()) != client->configuration().jid()))
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
        QStringList from = msg.from().split("/");
        DisplaTime(QString::number(time.toTime_t()));
        this->feedItem = new FeedBackListItem(from[1], content, enmu, this);
        this->listItem = new QListWidgetItem(ui->listWidget);
        this->listItem->setSizeHint(QSize(this->width() - 10, 60));
        ui->listWidget->setItemWidget(this->listItem, feedItem);
        ui->listWidget->scrollToBottom();
        this->connect(this->feedItem, &FeedBackListItem::sendItemSize, this, &GroupWindow::getItemSize);
    }
}

void GroupWindow::DispalyMessage(QString msg, QString time, QNChatMessage::User_Type type)
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

void GroupWindow::DisplaTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0) {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 300); // 两个消息相差一分钟
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

void GroupWindow::getItemSize(int w, int h)
{
    this->listItem->setSizeHint(QSize(w, h));
}

void GroupWindow::TurnModeRandom()
{
    this->status = GroupWindow::Mode::Random;
    this->room->setSubject("Mode::Random");
    for (auto i = this->member.begin(); i != this->member.end(); ++i) {
        if (this->room->participantPresence(i.key()).mucItem().role() == QXmppMucItem::Role::VisitorRole) {

            QXmppElementList membersEls;
            QXmppElement queryEl;
            QXmppElement itemEl;

            itemEl.setTagName("item");
            itemEl.setAttribute("nick", i.key().split('/')[1]);
            itemEl.setAttribute("role", "participant");

            queryEl.setTagName("query");
            queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
            queryEl.appendChild(itemEl);

            membersEls.append(queryEl);

            QXmppIq Iq(QXmppIq::Set);
            Iq.setTo(room->jid());
            Iq.setFrom(client->configuration().jidBare());
            Iq.setExtensions(membersEls);
            client->sendPacket(Iq);

        }
    }
}

void GroupWindow::KickMember()
{
    if (this->Member_Item == nullptr) return;
    QString nickname = this->Member_Item->text();

    this->room->ban(this->room->participantFullJid(RoomJid + "/" + nickname).split('/')[0], "kick");

    ui->memberList->removeItemWidget(this->Member_Item);
    delete this->Member_Item;
    this->Member_Item = nullptr;

    auto i = this->member.find(RoomJid + "/" + nickname);
    if (i != this->member.end()) {
        this->member.erase(i);
    }

}

void GroupWindow::BanVoice()
{
    if (this->Member_Item == nullptr) return;
    QString nickname = this->Member_Item->text();
    this->Member_Item = nullptr;


    QXmppElementList membersEls;
    QXmppElement queryEl;
    QXmppElement itemEl;

    itemEl.setTagName("item");
    itemEl.setAttribute("nick", nickname);
    itemEl.setAttribute("role", "visitor");

    queryEl.setTagName("query");
    queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
    queryEl.appendChild(itemEl);

    membersEls.append(queryEl);

    QXmppIq Iq(QXmppIq::Set);
    Iq.setTo(room->jid());
    Iq.setFrom(client->configuration().jidBare());
    Iq.setExtensions(membersEls);
    client->sendPacket(Iq);
}

void GroupWindow::GiveVoice()
{
    if (this->Member_Item == nullptr) return;
    QString nickname = this->Member_Item->text();
    this->Member_Item = nullptr;

    QXmppElementList membersEls;
    QXmppElement queryEl;
    QXmppElement itemEl;

    itemEl.setTagName("item");
    itemEl.setAttribute("nick", nickname);
    itemEl.setAttribute("role", "participant");

    queryEl.setTagName("query");
    queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
    queryEl.appendChild(itemEl);

    membersEls.append(queryEl);

    QXmppIq Iq(QXmppIq::Set);
    Iq.setTo(room->jid());
    Iq.setFrom(client->configuration().jidBare());
    Iq.setExtensions(membersEls);
    client->sendPacket(Iq);

}

void GroupWindow::SpecifyVoice()
{
    if (this->Member_Item == nullptr) return;
    this->status = Mode::Specity;
    QString nickname = this->Member_Item->text();
    //this->Member_Item = nullptr;

    for (auto i = this->member.begin(); i != this->member.end(); ++i) {
        if (i->first == this->Member_Item) {
            if (this->room->participantPresence(RoomJid + "/" + nickname).mucItem().role() == QXmppMucItem::Role::VisitorRole
                    || this->room->participantPresence(RoomJid + "/" + nickname).mucItem().role() == QXmppMucItem::Role::NoRole) {
                QXmppElementList membersEls;
                QXmppElement queryEl;
                QXmppElement itemEl;

                itemEl.setTagName("item");
                itemEl.setAttribute("nick", nickname);
                itemEl.setAttribute("role", "participant");

                queryEl.setTagName("query");
                queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
                queryEl.appendChild(itemEl);

                membersEls.append(queryEl);

                QXmppIq Iq(QXmppIq::Set);
                Iq.setTo(room->jid());
                Iq.setFrom(client->configuration().jidBare());
                Iq.setExtensions(membersEls);
                client->sendPacket(Iq);
            }
        } else if (this->room->participantPresence(RoomJid + "/" + i->first->text()).mucItem().role() == QXmppMucItem::Role::ParticipantRole) {
            QXmppElementList membersEls;
            QXmppElement queryEl;
            QXmppElement itemEl;

            itemEl.setTagName("item");
            itemEl.setAttribute("nick", i.key().split('/')[1]);
            itemEl.setAttribute("role", "visitor");

            queryEl.setTagName("query");
            queryEl.setAttribute("xmlns", "http://jabber.org/protocol/muc#admin");
            queryEl.appendChild(itemEl);

            membersEls.append(queryEl);

            QXmppIq Iq(QXmppIq::Set);
            Iq.setTo(room->jid());
            Iq.setFrom(client->configuration().jidBare());
            Iq.setExtensions(membersEls);
            client->sendPacket(Iq);
        } else {
            continue;
        }
    }
    this->Member_Item = nullptr;
}

void GroupWindow::subjectChangedHandler(const QString &subject)
{
    if (subject == tr("Mode::Order")) {
        this->status = Mode::Order;
        ui->textEdit->show();
    } else if (subject == tr("Mode::Random")) {
        this->order.clear();
        this->next = nullptr;
        this->status = Mode::Random;
        ui->textEdit->hide();
        if (room->participantPresence(RoomJid + "/" + room->nickName()).mucItem().role() == QXmppMucItem::ModeratorRole) {
            ui->pushButton_3->show();
        }
    } else if (subject == "Mode::Specity") {
        this->status = Mode::Specity;
    } else {
        ui->textEdit->setText("现在由：" + subject + "发言");
    }
}
