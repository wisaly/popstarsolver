#include "widget.h"
#include <QApplication>
#include "board.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    Board b;
//    b.rand();
//    b.solve();

    Widget w;
    w.show();

    return a.exec();
}
