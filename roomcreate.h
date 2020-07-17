#ifndef ROOMCREATE_H
#define ROOMCREATE_H

#include "basewindow.h"
#include "global.h"
#include "client.h"
#include <QRegExpValidator>
#include <QMessageBox>

namespace Ui {
class RoomCreate;
}

class RoomCreate : public BaseWindow
{
    Q_OBJECT

public:
    explicit RoomCreate(QWidget *parent = nullptr);
    ~RoomCreate();

private:
    Ui::RoomCreate *ui;
    QXmppClient *client;
    QString Jid;
    QXmppMucManager *MucMng;
    QXmppBookmarkManager *BookMng;
    QXmppDiscoveryManager *DisMng;
    void Create();
private slots:
    void on_cancel_clicked();
    void on_confirm_clicked();
    void infoReceivedHandler(const QXmppDiscoveryIq &info);
};

#endif // ROOMCREATE_H
