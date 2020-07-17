#include "addfrinedorgroup.h"
#include "ui_addfrinedorgroup.h"

AddFrinedorGroup::AddFrinedorGroup(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::AddFrinedorGroup),
    client(GobalClient.getClient()),
    RstMng(GobalClient.getClient()->findExtension<QXmppRosterManager>()),
    BookMng(GobalClient.getClient()->findExtension<QXmppBookmarkManager>()),
    DisMng(GobalClient.getClient()->findExtension<QXmppDiscoveryManager>()),
    firstAdd(false)
{
    ui->setupUi(this);
    this->initStyle();
    this->connect(ui->AddFriendButton, &QPushButton::clicked, this, &AddFrinedorGroup::AddFrined);
    this->connect(ui->AddGroupButton, &QPushButton::clicked, this, &AddFrinedorGroup::AddGroup);
    this->connect(DisMng, &QXmppDiscoveryManager::infoReceived, this, &AddFrinedorGroup::infoReceivedHandler);
}

AddFrinedorGroup::~AddFrinedorGroup()
{
    delete ui;
}

void AddFrinedorGroup::AddFrined()
{
    QString Jid = ui->lineEdit->text() + "@120.26.43.184";
    if (Jid.simplified().length() == 0) {
        QMessageBox::critical(this,"输入非法","您输入的ID非法！");
    } else {
        RstMng->subscribe(Jid);
        QMessageBox::information(this, "好友申请", "好友申请已发送");
    }
}

void AddFrinedorGroup::AddGroup()
{
    RoomJid = ui->lineEdit->text();
    QString Server = "@conference.120.26.43.184";
    QString Service = RoomJid + Server;
    Service = Service.toLower();
    QRegExp regx("[a-zA-Z0-9\\x4e00-\\x9fa5]+");

    if (!regx.exactMatch(RoomJid)) {
        QMessageBox::critical(this,"输入非法","您输入的群聊名称非法!");
    } else {
        DisMng->requestInfo(Service);
    }
}

void AddFrinedorGroup::infoReceivedHandler(const QXmppDiscoveryIq &iq)
{
    QList<QXmppDiscoveryIq::Identity> identities = iq.identities();
    if (identities.size()) {
        QXmppDiscoveryIq::Identity identity = identities.at(0);
        if (identity.name() == RoomJid) {
            QList<QXmppBookmarkConference> conferenceList = BookMng->bookmarks().conferences();
            foreach(QXmppBookmarkConference Conference, conferenceList) {
                if (Conference.jid() == RoomJid + "@conference.120.26.43.184") {
                    QMessageBox::critical(this,"加入失败","该群聊已加入！");
                    return;
                }
            }
            QXmppPresence msg;
            QString to = RoomJid + "@conference.120.26.43.184" + "/" + GobalClient.getClient()->configuration().jidBare();
            QString from = GobalClient.getClient()->configuration().jidBare();
            this->client->sendPacket(msg);
            QXmppBookmarkConference *bookmark = new QXmppBookmarkConference;
            QXmppBookmarkSet set = BookMng->bookmarks();
            bookmark->setJid(RoomJid + "@conference.120.26.43.184");
            bookmark->setName(RoomJid);
            bookmark->setAutoJoin(true);
            bookmark->setNickName(client->configuration().jidBare());
            conferenceList.append(*bookmark);
            set.setConferences(conferenceList);
            BookMng->setBookmarks(set);
            QMessageBox::information(this, "申请群聊", "已顺利加入");
        } else {
            QMessageBox::critical(this,"加入失败","该群聊不存在！");
        }
    } else {
        QMessageBox::critical(this,"加入失败","该群聊不存在！");
    }
}
