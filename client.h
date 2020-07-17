#ifndef CLIENT_H
#define CLIENT_H

#include <qxmpp/QXmppClient.h>

class Client
{
public:
    Client();
    ~Client();
    QXmppClient *getClient();
private:
    QXmppClient *client;
};

#endif // CLIENT_H
