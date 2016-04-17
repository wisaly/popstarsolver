#ifndef BOARD_H
#define BOARD_H

#include "move.h"

class Board
{
public:
    // board size, colors
    enum { N = 10, NC = 5, BONUS = 1000};

    Board();
    Board(const Board &o);

    // get color at position
    inline int at(int x, int y) { return d_[x][y]; }
    // whether the board is empty or not
    inline bool isEmpty() { return d_[0][N - 1] == 0; }
    // get available moves
    QList<Move> moves();
    QList<Move> tabuMoves(int tabu);
    // get end-game score
    int endScore();
    // get the zobrist hash of the board
    long hash();
    // make a move
    int step(Move &move);
    // upper score limit of current board
    int upperScore();
private:
    static void mark(int c[N][N], int x, int y, Move &move);
    static void copy(int dst[N][N], const int src[N][N]);
    static void fall(int d[N][N], Move &move);
private:
    int d_[N][N];
};

#endif // BOARD_H
