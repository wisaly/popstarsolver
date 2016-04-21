#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "board.h"
#include "mcts.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void on_btnRand_clicked();
    void on_btnSolve_clicked();
    void on_btnStep_clicked();
    void on_board_currentChanged(int score);
    void on_board_scoreChanged(int score, int steps);
    void on_board_gameOver();

private:
    Ui::Widget *ui;
    Board board_;
    int step_;
    MCTS mcts_;
};

#endif // WIDGET_H
