#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include "gameboard.h"

class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoardWidget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void setBoard(GameBoard *board) { board_ = board; }
signals:

public slots:

private:
    GameBoard *board_;
    int unitWidth_ = 0;
    int unitHeight_ = 0;
};

#endif // BOARDWIDGET_H
