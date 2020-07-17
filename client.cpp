#include "client.h"

Client::Client() :
    client(new QXmppClient)
{

}

Client::~Client()
{
    delete client;
}

QXmppClient *Client::getClient()
{
    return this->client;
}
