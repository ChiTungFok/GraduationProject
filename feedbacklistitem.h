#ifndef FEEDBACKLISTITEM_H
#define FEEDBACKLISTITEM_H

#include <QWidget>
#include <QPainter>
#include <QEvent>
#include <QDateTime>

#define     COLOR_WHITE         Qt::white
#define     ITEM_HEIGHT         40
#define     ITEM_SPACE          5
#define     HeaderBgColor       QColor(219, 238, 252)
#define     HeaderTextColor     QColor(55, 100, 151)
#define     COLOR_BLACK         Qt::black
#define     HEAD_W_H            40

enum Orientation{
    None,
    Left,
    Right
};

class FeedBackListItem : public QWidget
{
    Q_OBJECT
public:
    explicit FeedBackListItem(QString from, QString text, int Ori, QWidget *parent = nullptr);
    enum Orientation{
        None,
        Left,
        Right
    };

private:
    QString     m_from;
    QString     m_text;
    int         m_width;
    int         m_oritation;
    QWidget     *m_parent;
    QSize       getItemSize(int ,int );
    void        drawBg(QPainter* painter);
    void        drawItems(QPainter* painter);
signals:
    void sendItemSize(int,int);
protected:
    bool eventFilter(QObject *, QEvent *);

};

#endif // FEEDBACKLISTITEM_H
