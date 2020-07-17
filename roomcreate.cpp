#include "roomcreate.h"
#include "ui_roomcreate.h"

RoomCreate::RoomCreate(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::RoomCreate),
    client(GobalClient.getClient())
{
    ui->setupUi(this);
    this->MucMng = this->client->findExtension<QXmppMucManager>();
    this->BookMng = this->client->findExtension<QXmppBookmarkManager>();
    this->DisMng = this->client->findExtension<QXmppDiscoveryManager>();
    this->connect(this->DisMng, &QXmppDiscoveryManager::infoReceived, this, &RoomCreate::infoReceivedHandler);
    this->initStyle();
}

RoomCreate::~RoomCreate()
{
    delete ui;
}

void RoomCreate::on_cancel_clicked()
{
    this->close();
}

void RoomCreate::on_confirm_clicked()
{
    this->Jid = ui->lineEdit->text();
    QRegExp regx("[a-zA-Z0-9\\x4e00-\\x9fa5]+");
    if (!regx.exactMatch(Jid)) {
        QMessageBox::critical(this,"输入非法","您输入的群聊名称非法!");
    } else {
        this->DisMng->requestInfo(Jid);
    }
}

void RoomCreate::infoReceivedHandler(const QXmppDiscoveryIq &info)
{
    QList<QXmppDiscoveryIq::Identity> identities = info.identities();
    if (identities.size() == 0) {
        Create();
    } else {
        foreach(QXmppDiscoveryIq::Identity identity, identities) {
            if (identity.name() == this->Jid) {
                QMessageBox::critical(this,"创建失败","该群聊已存在！");
                return;
            }
        }
        Create();
    }
}

void RoomCreate::Create()
{
    QString server = "120.26.43.184";
    QString service = this->Jid + "@conference." + server;

    QXmppMucRoom *room = MucMng->addRoom(service);

    if (room) {
        QStringList nickname = client->configuration().jid().split("@");
        room->setNickName(nickname[0]);
        room->join();


        QXmppDataForm form(QXmppDataForm::Submit);
        QList<QXmppDataForm::Field> fields;
        {
           QXmppDataForm::Field field(QXmppDataForm::Field::HiddenField);
           field.setKey("FORM_TYPE");
           field.setValue("http://jabber.org/protocol/muc#roomconfig");
           fields.append(field);
        }

        {
            QXmppDataForm::Field field;
            field.setKey("muc#roomconfig_roomname");
            field.setValue(this->Jid);
            fields.append(field);
        }

        {
           QXmppDataForm::Field field(QXmppDataForm::Field::BooleanField);
           field.setKey("muc#roomconfig_persistentroom");
           field.setValue(true);
           fields.append(field);
        }

        {
           QXmppDataForm::Field field(QXmppDataForm::Field::BooleanField);
           field.setKey("muc#roomconfig_moderatedroom");
           field.setValue(true);
           fields.append(field);
        }

        {
            QXmppDataForm::Field field(QXmppDataForm::Field::BooleanField);
            field.setKey("muc#roomconfig_allowinvites");
            field.setValue(true);
            fields.append(field);
        }

        {
            QXmppDataForm::Field field(QXmppDataForm::Field::ListMultiField);
            field.setKey("muc#roomconfig_getmemberlist");
            field.setValue(QXmppMucItem::MemberAffiliation);
            fields.append(field);
        }

        form.setFields(fields);
        room->setConfiguration(form);

        QXmppBookmarkConference *bookmark = new QXmppBookmarkConference;
        QXmppBookmarkSet set = BookMng->bookmarks();
        QList<QXmppBookmarkConference> conferenceList = BookMng->bookmarks().conferences();
        bookmark->setJid(this->Jid + "@conference." + server);
        bookmark->setName(this->Jid);
        bookmark->setAutoJoin(true);
        bookmark->setNickName(nickname[0]);
        conferenceList.append(*bookmark);
        set.setConferences(conferenceList);
        BookMng->setBookmarks(set);

        QMessageBox::information(this, "创建成功", "群聊创建成功");
    }


}
