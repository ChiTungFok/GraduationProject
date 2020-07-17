#include "feedbacklistitem.h"

FeedBackListItem::FeedBackListItem(QString from, QString text, int Ori, QWidget *parent) :
    QWidget(parent),
    m_from(from),
    m_text(text),
    m_width(0),
    m_oritation(Ori)
{
    m_parent = parent;
    this->installEventFilter(this);
}

bool FeedBackListItem::eventFilter(QObject *obj, QEvent *event)
{
    if((obj == this)&&(event->type()==QEvent::Paint))
    {
        QPainter painter(this);
        painter.setRenderHints(QPainter::HighQualityAntialiasing|QPainter::Antialiasing);
        drawBg(&painter);
        drawItems(&painter);
        return QWidget::eventFilter(obj,event);
    }else{
        return false;
    }
}

//BG COLOR
void FeedBackListItem::drawBg(QPainter *painter)
{
    painter->save();
    // painter->setPen(Qt::NoPen);
    painter->setBrush(COLOR_WHITE);
    painter->setPen(COLOR_WHITE);
    painter->drawRect(rect());
    painter->restore();
}

QSize FeedBackListItem::getItemSize(int width,int height)
{
    return QSize(width,height);
}

void FeedBackListItem::drawItems(QPainter *painter)
{
    // 绘制item区域
    painter->save();

    int nItemY = 0;
    int nWidth = this->width();
    nWidth = (0 == nWidth % 2) ? nWidth : nWidth + 1;

    //top Y
    QPointF topLeft(0,nItemY+30);
    QPointF bottomRight(nWidth,nItemY + ITEM_HEIGHT + ITEM_SPACE);
    QRectF  ItemRect(topLeft,bottomRight);

    painter->save();
    QTransform t;
    t.translate(ItemRect.center().x(),ItemRect.center().y());
    //进入样式
    // t.scale(m_IIVec.at(nIndex).getZoomingFactor(),m_IIVec.at(nIndex).getZoomingFactor());
    painter->setTransform(t);

    QPointF rectTopLeft;
    QPointF rectBottomRight;
    QRectF textRect(rectTopLeft,rectBottomRight);

    QFont font("幼圆", 10);
    painter->setFont(font);

    QPainterPath path;
    // 计算文字的宽度
    QFontMetrics fm(font);
    int pixelsWide = fm.width(m_text);
    int pixelsHigh = fm.height();
    int m_wf = nWidth * 2 / 3;
    pixelsHigh = pixelsWide < (m_wf*8/9) ?  ITEM_HEIGHT : (((pixelsWide / (nWidth / 2)) + 2) * ITEM_HEIGHT*2/5);
    pixelsWide = pixelsWide < (m_wf*8/9) ?  pixelsWide  : (m_wf*8/9);

    if (Right == m_oritation)
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(HeaderBgColor);
        // 绘制边框  头像边框
        painter->drawRoundedRect(nWidth / 2 - 55, -ITEM_HEIGHT / 2, HEAD_W_H, HEAD_W_H, 2, 2);
        // 绘制头像
        painter->setPen(HeaderTextColor);
        //painter->drawPixmap(nWidth / 2 - 54, -ITEM_HEIGHT / 2 + 1, 48, 48, QPixmap(""));
        painter->drawText(nWidth / 2 - 54, -ITEM_HEIGHT / 2 + 1, HEAD_W_H, HEAD_W_H,Qt::AlignCenter,m_from);
        painter->restore();


        // 计算右边的宽度x
        int nX = (nWidth / 2) - 85 - pixelsWide;
        if (nX < 0) {
            nX = -pixelsWide - 85 + nWidth / 2;
        }
        painter->save();

        // 计算气泡框
        textRect = QRectF(nX, -ITEM_HEIGHT / 2, pixelsWide + 20, pixelsHigh);
#if 1
        // 计算气泡右边的三角   三角内颜色
        path.addRoundedRect(textRect, 3, 3);    //聊天框弧度
        path.moveTo(nWidth / 2 - 65, -ITEM_HEIGHT / 2 + 12);
        path.lineTo(nWidth / 2 - 55, -ITEM_HEIGHT / 2 + 18);
        path.lineTo(nWidth / 2 - 65, -ITEM_HEIGHT / 2 + 21);
        painter->setPen(QColor(140, 170, 202));
        painter->drawPath(path);

        //painter->fillPath(path,HeaderTextColor);
#else
        painter->drawPixmap(nX + 10, -ITEM_HEIGHT / 2, pixelsWide, pixelsHigh, QPixmap(""));
#endif
        painter->restore();
        painter->setPen(Qt::white);
        // 重新调整文字区域
        textRect = QRectF(nX + 10, -ITEM_HEIGHT / 2, pixelsWide, pixelsHigh);
    }
    else {

        // 绘制气泡区域
        painter->save();
        textRect = QRectF(-nWidth / 2 + 59, -ITEM_HEIGHT / 2, pixelsWide + 20, pixelsHigh);
        // 左边三角
        path.addRoundedRect(textRect, 3, 3);
        path.moveTo(-nWidth / 2 + 59, -ITEM_HEIGHT / 2 + 12);
        path.lineTo(-nWidth / 2 + 49, -ITEM_HEIGHT / 2 + 18);
        path.lineTo(-nWidth / 2 + 59, -ITEM_HEIGHT / 2 + 21);
        painter->setPen(QColor(140, 170, 202));
        painter->drawPath(path);
        painter->restore();

        // 绘制头像
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(HeaderBgColor);
        QRectF  leftRect(-nWidth / 2 + 5, -ITEM_HEIGHT / 2, HEAD_W_H, HEAD_W_H);
        painter->drawRect(leftRect);
        painter->setPen(HeaderTextColor);
        painter->drawText(leftRect, Qt::AlignCenter,m_from);
        painter->restore();

        painter->setPen(COLOR_BLACK);
        // 绘制文字区域
        textRect = QRectF(-nWidth / 2 + 59 + 10, -ITEM_HEIGHT / 2, pixelsWide, pixelsHigh);
    }

    //设置text颜色
    painter->setPen(Qt::red);
    painter->drawText(textRect, m_text, Qt::AlignVCenter |Qt::AlignLeft);
    painter->restore();

    /// increment nItemY    item高度设置
    if(pixelsHigh <= HEAD_W_H){
        pixelsHigh = HEAD_W_H;
        nItemY += pixelsHigh + ITEM_SPACE;   //head height + ITEMSPACING
    }else{
        nItemY += pixelsHigh + ITEM_SPACE;
    }
    //QSize(width(),height)
    emit sendItemSize(width(),pixelsHigh + ITEM_SPACE+30);
}
