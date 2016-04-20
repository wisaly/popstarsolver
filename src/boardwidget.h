#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include "board.h"

class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoardWidget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void setBoard(Board *board) { board_ = board; }
private:
signals :
    void currentChanged(int score);
    void scoreChanged(int score, int steps);
    void gameOver();
public slots:

private:
    Board *board_;
    int unitWidth_ = 0;
    int unitHeight_ = 0;
    Move currentMove_;
    int total_ = 0;
    int steps_ = 0;
};

#endif // BOARDWIDGET_H
