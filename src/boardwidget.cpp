#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include "boardwidget.h"

BoardWidget::BoardWidget(QWidget *parent) :
    QWidget(parent),
    board_(0)
{
}

void BoardWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (board_ == 0)
        return;

    QPainter painter(this);

    unitWidth_ = width() / Board::N;
    unitHeight_ = height() / Board::N;

    for (int x = 0;x < width();x += unitWidth_)
        painter.drawLine(x,0,x,height());
    for (int y = 0;y < height();y += unitHeight_)
        painter.drawLine(0,y,width(),y);

    painter.setFont(QFont("Consolas",15));
    for (int i = 0;i < Board::N;i++)
    {
        for (int j = 0;j < Board::N;j++)
        {
            QRect rc(i*unitWidth_,j*unitHeight_,
                     unitWidth_,unitHeight_);

            int n = board_->at(j,i);
            painter.fillRect(rc,Board::color(n));
            painter.drawText(rc,
                             Qt::AlignCenter,
                             QString::number(n));
        }
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (unitWidth_ ==0 || unitHeight_ == 0)
        return;

    int x = event->pos().x() / unitWidth_;
    int y = event->pos().y() / unitHeight_;

    qDebug() << "x:" << x << ",y:" << y;
    board_->select(x,y);

    update();
}
