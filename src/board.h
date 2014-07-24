#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QList>

class Board
{
public:
    Board();
    enum{ N = 6};

    class Node
    {
    public:
        Node(int xx,int yy):x(xx),y(yy){}
        int x;
        int y;
    };
    class Move
    {
    public:
        Move(Node node,int score):n(node),s(score) {}
        Node n;
        int s;
    };

    int at(int x,int y) { return d_[x][y]; }
    int (*d())[N] { return d_; }
    static Qt::GlobalColor clr(int n);

    void rand();
    int detect();
    void fall(int c[N][N], int xfrom, int xto);

    void solve();
    void solve(int score, int d[N][N], QList<Move> &moves);
    QList<Move> &solution() { return bestMove_; }

    void dbgPut(int c, int a[N][N]);
    void dbgPut(QList<Move> &moves);
    void mark(int c[N][N], int x, int y, QList<Node> &ss);
private:
    int d_[N][N];
    int solutionCount_;
    int bestScore_;
    QList<Move> bestMove_;
};

#endif // BOARD_H
