#ifndef GLOBAL_H
#define GLOBAL_H

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppVCardIq.h>
#include <qxmpp/QXmppVCardManager.h>
#include <qxmpp/QXmppBookmarkManager.h>
#include <qxmpp/QXmppBookmarkSet.h>
#include <qxmpp/QXmppMucIq.h>
#include <qxmpp/QXmppMucManager.h>
#include <qxmpp/QXmppDiscoveryIq.h>
#include <qxmpp/QXmppDiscoveryManager.h>
#include <qxmpp/QXmppRegisterIq.h>
#include <qxmpp/QXmppRegistrationManager.h>
#include <qxmpp/QXmppRosterManager.h>
#include <qxmpp/QXmppRosterIq.h>
#include <qxmpp/QXmppMessage.h>
#include <qxmpp/QXmppMessageReceiptManager.h>
#include <qxmpp/QXmppUtils.h>
#include <qxmpp/QXmppTransferManager.h>
#include "client.h"

extern Client GobalClient;
extern bool is_login_success;

#endif // GLOBAL_H
