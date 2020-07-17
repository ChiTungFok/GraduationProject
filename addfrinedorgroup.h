#ifndef ADDFRINEDORGROUP_H
#define ADDFRINEDORGROUP_H

#include "basewindow.h"
#include "global.h"
#include "client.h"
#include <QRegExpValidator>
#include <QMessageBox>

namespace Ui {
class AddFrinedorGroup;
}

class AddFrinedorGroup : public BaseWindow
{
    Q_OBJECT

public:
    explicit AddFrinedorGroup(QWidget *parent = nullptr);
    ~AddFrinedorGroup();

private:
    Ui::AddFrinedorGroup *ui;
    QXmppClient *client;
    QXmppRosterManager *RstMng;
    QXmppBookmarkManager *BookMng;
    QXmppDiscoveryManager *DisMng;
    bool firstAdd;
    QString RoomJid;
private slots:
    void AddFrined();
    void AddGroup();
    void infoReceivedHandler(const QXmppDiscoveryIq &iq);
};

#endif // ADDFRINEDORGROUP_H
