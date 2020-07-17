#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QTimer>
#include <QRegExpValidator>
#include <QEventLoop>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include "basewindow.h"
#include "registwindow.h"
#include "global.h"

namespace Ui {
class loginwindow;
}

class loginwindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit loginwindow(QWidget *parent = nullptr);
    ~loginwindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::loginwindow *ui;
    QXmppClient *client;
    int  width, heigth;
    void login();
    void Success();
    void regist();
    void windowClosed();


};

#endif // LOGINWINDOW_H
