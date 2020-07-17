#include "loginwindow.h"
#include "registwindow.h"
#include "mainwindow.h"
#include <QApplication>

bool is_login_success = false;
Client GobalClient;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loginwindow login;
    login.show();
    return a.exec();
}
