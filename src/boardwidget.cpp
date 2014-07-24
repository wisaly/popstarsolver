#include <QPaintEvent>
#include <QPainter>
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

    int unitWidth = width() / Board::N;
    int unitHeight = height() / Board::N;

    for (int x = 0;x < width();x += unitWidth)
        painter.drawLine(x,0,x,height());
    for (int y = 0;y < height();y += unitHeight)
        painter.drawLine(0,y,width(),y);

    painter.setFont(QFont("Consolas",15));
    for (int i = 0;i < Board::N;i++)
    {
        for (int j = 0;j < Board::N;j++)
        {
            QRect rc(i*unitWidth,j*unitHeight,
                     unitWidth,unitHeight);

            int n = board_->at(j,i);
            painter.fillRect(rc,Board::clr(n));
            painter.drawText(rc,
                             Qt::AlignCenter,
                             QString::number(n));
        }
    }
}
