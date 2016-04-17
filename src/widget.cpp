#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->board->setBoard(&board_);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnRand_clicked()
{
    board_.rand();
    ui->board->update();
}

void Widget::on_btnSolve_clicked()
{
    board_.solve();
    step_ = 0;
}

void Widget::on_btnStep_clicked()
{
    if (step_ >= board_.solution().count())
    {
        ui->btnStep->setText(QString("step over"));
        return;
    }
    ui->btnStep->setText(QString("step %1").arg(step_));

    GameBoard::Move m = board_.solution()[step_];
    board_.select(m.n.x,m.n.y);
    step_++;

    ui->board->update();
}
