#ifndef WIDGET_H
#define WIDGET_H

#include "gameboard.h"

#include <QWidget>

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

private:
    Ui::Widget *ui;
    GameBoard board_;
    int step_;
};

#endif // WIDGET_H
