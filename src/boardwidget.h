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

    void setBoard(Board *board) { board_ = board; }
signals:

public slots:

private:
    Board *board_;
};

#endif // BOARDWIDGET_H
