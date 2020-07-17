#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolButton>
#include <QMouseEvent>
#include <QStyle>

class BaseWindow : public QWidget
{
    Q_OBJECT
public:
    BaseWindow(QWidget *parent = nullptr);

    void initStyle();

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QPoint lastPos;

public slots :
    virtual void windowClosed();
    virtual void windowMinimized();

};

#endif // BASEWINDOW_H
