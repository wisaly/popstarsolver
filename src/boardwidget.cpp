#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include "boardwidget.h"

const Qt::GlobalColor g_colors[6] =
{ Qt::white, Qt::red, Qt::green, Qt::yellow, Qt::blue, Qt::magenta };

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
    int boderWidth = unitWidth_ / 10;
    int boderHeight = unitHeight_ / 10;

    painter.setPen(Qt::lightGray);

    for (int x = 0;x < width();x += unitWidth_)
        painter.drawLine(x,0,x,height());
    for (int y = 0;y < height();y += unitHeight_)
        painter.drawLine(0,y,width(),y);

    painter.setPen(Qt::black);
    painter.setFont(QFont("Consolas",15));
    for (int i = 0;i < Board::N;i++)
    {
        for (int j = 0;j < Board::N;j++)
        {
            QRect rc(j*unitWidth_,i*unitHeight_,
                     unitWidth_,unitHeight_);

            QRect cell = rc.marginsRemoved(QMargins(boderWidth, boderHeight, boderWidth, boderHeight));
            if (currentMove_.isIn(j, i))
                painter.fillRect(rc, Qt::black);

            int n = board_->at(j,i);
            painter.fillRect(cell, g_colors[n]);
            painter.drawText(cell,
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

    if (board_->at(x, y) == 0)
    {
        currentMove_.clear();
        return;
    }

    if (currentMove_.isIn(x, y))
    {
        total_ += board_->step(currentMove_);
        steps_++;

        currentMove_.clear();

        emit currentChanged(0);
        emit scoreChanged(total_, steps_);

        if (board_->moves().count() == 0)
        {
            total_ += board_->endScore();
            emit gameOver();
        }
    }
    else
    {
        currentMove_ = board_->moveAt(x, y);
        emit currentChanged(currentMove_.score());
    }

    update();
}
