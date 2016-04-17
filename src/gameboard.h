#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QList>
#include <QHash>

class GameBoard
{
public:
    GameBoard();
    enum{ N = 9, M = 5};

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

    int at(int x, int y) { return d_[x][y]; }
    static Qt::GlobalColor color(int n);

    void rand();
    void select(int x, int y);

    void solve();
    QList<Move> &solution() { return bestMove_; }

    void averageCells();

private:
    void dbgPut(int c, int a[N][N]);
    void dbgPut(QList<Move> &moves);
    void fall(int c[N][N], int xfrom, int xto);
    void mark(int c[N][N], int x, int y, QList<Node> &ss);
    int solve(int score, int d[N][N], QList<Move> &moves);
    bool worthSearch(int score, int c[N][N]);
    QByteArray stateSign(int c[N][N]);
private:
    int d_[N][N];
    int solutionCount_;
    int bestScore_;
    QList<Move> bestMove_;
    QHash<QByteArray,int> searchHistory_;
};

#endif // BOARD_H
