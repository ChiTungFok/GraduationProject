#ifndef REGISTWINDOW_H
#define REGISTWINDOW_H

#include <QRegExpValidator>
#include <QMessageBox>
#include"basewindow.h"
#include "global.h"

namespace Ui {
class RegistWindow;
}

class RegistWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit RegistWindow(QWidget *parent = nullptr);
    ~RegistWindow();

public slots:
    void Regist();
    void Reset();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::RegistWindow *ui;
    QXmppClient *Regist_Client;

private slots:
    void iqReceived(const QXmppIq &iq);
};

#endif // REGISTWINDOW_H
