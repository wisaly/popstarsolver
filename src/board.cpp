#include "board.h"
#include <QQueue>
#include <cstring>

// zobrist hash random number cahce
class ZobristMetrix
{
public:
    ZobristMetrix()
    {
        for (int i = 0; i < Board::N; i++)
            for (int j = 0; j < Board::N; j++)
                for (int c = 0; c < Board::NC + 1; c++)
                    d_[i][j][c] = qrand();
    }
    long d_[Board::N][Board::N][Board::NC + 1];
};

Q_GLOBAL_STATIC(ZobristMetrix, g_zobristMetrix)

Board::Board()
{
    std::memset(d_, 0, sizeof(int)* N * N);
}

Board::Board(const Board &o)
{
    copy(d_, o.d_);
}

void Board::mark(int c[N][N], int x, int y, Move &move)
{
    move.cell_.clear();
    move.color_ = c[y][x];
    move.isVS_ = true;

    if (move.color_ == 0)
        return;

    QQueue<Move::Node> q; // search quque
    q.enqueue(Move::Node(x,y));
    while(!q.empty())
    {
        Move::Node h = q.dequeue();
        move.cell_.append(h);

        c[h.y_][h.x_] = 0;

        // left
        if (h.x_ > 0 && c[h.y_][h.x_ - 1] == move.color_)
        {
            q.enqueue(Move::Node(h.x_ - 1, h.y_));
            move.isVS_ = false;
        }
        // right
        if (h.x_ < N - 1 && c[h.y_][h.x_ + 1] == move.color_)
        {
            q.enqueue(Move::Node(h.x_ + 1, h.y_));
            move.isVS_ = false;
        }
        // up
        if (h.y_ > 0 && c[h.y_ - 1][h.x_] == move.color_)
            q.enqueue(Move::Node(h.x_, h.y_ - 1));
        // down
        if (h.y_ < N - 1 && c[h.y_ + 1][h.x_] == move.color_)
            q.enqueue(Move::Node(h.x_, h.y_ + 1));
    }
}

void Board::copy(int dst[N][N], const int src[N][N])
{
    std::memcpy(dst, src, sizeof(int) * N * N);
}

void Board::fall(int d[N][N], Move &move)
{
    int xfrom = N - 1;
    int xto = 0;

    // remove target
    for (Move::Node &n : move.cell_)
    {
        d[n.y_][n.x_] = 0;

        if (n.x_ < xfrom)
            xfrom = n.x_;
        if (n.x_ > xto)
            xto = n.x_;
    }

    // fall down
    for (int x = xfrom; x <= xto; x++)
    {
        for (int y = N - 1; y > 0;y--)
        {
            // fast skip empty from bottom
            if (d[y][x] != 0)
                continue;

            int e = 0; // empty count
            for (int yy = y; yy >= 0 && d[yy][x] == 0; yy--)
                e++;

            // reach top border
            if (y < e)
                break;

            // move down
            for (int yy = y; yy >= e; yy--)
                d[yy][x] = d[yy - e][x];
            // fill empty
            for (int yy = e - 1; yy >= 0; yy--)
                d[yy][x] = 0;
        }
    }
    // fall left
    for (int x = xfrom; x <= xto; x++)
    {
        // fast skip
        if (d[N - 1][x] != 0)
            continue;

        int e = 0;
        for (int xx = x; xx <= xto && d[N - 1][xx] == 0; xx++)
            e++;

        // reach right border
        if (x + e >= N)
            break;

        // move left
        for (int xx = x; xx < N - e; xx++)
            for (int y = 0; y < N; y++)
                d[y][xx] = d[y][xx + e];
        // fill empty
        for (int xx = N - e; xx < N; xx++)
            for (int y = 0; y < N; y++)
                d[y][xx] = 0;
    }
}

QList<Move> Board::moves()
{
    QList<Move> result;
    int t[N][N];
    copy(t,d_);

    Move m;
    for (int y = N - 1;y >= 0;y--)
    {
        for (int x = 0;x < N;x++)
        {
            if (t[y][x] == 0)
                continue;

            mark(t, x, y, m);

            if (m.size() < 2)
                continue;

            result.append(m);
        }
    }

    return result;
}

QList<Move> Board::tabuMoves(int tabu)
{
    QList<Move> result;
    int t[N][N];
    copy(t,d_);

    Move m;
    for (int y = N - 1;y >= 0;y--)
    {
        for (int x = 0;x < N;x++)
        {
            if (t[y][x] == 0 || t[y][x] == tabu)
                continue;

            mark(t, x, y, m);

            if (m.size() < 2)
                continue;

            // perform VS-pruning
            if (m.isVS_ &&
                (y == 0 || t[y - 1][x] == 0) &&
                (y < N - 1 || x == 0))
                continue;

            result.append(m);
        }
    }

    if (!result.isEmpty() || tabu == 0)
        return result;

    return tabuMoves(0);
}

int Board::endScore()
{
    if (isEmpty())
        return Move::endScore(0);

    int score = 0;
    int t[N][N];
    int rest = 0;
    copy(t, d_);

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (t[i][j])
            {
                Move m;
                mark(t, j, i, m);
                if (m.size() == 1)
                    rest++;
                else
                    score += m.score();
            }
        }
    }

    score += Move::endScore(rest);

    return score;
}

long Board::hash()
{
    long result = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            result ^= g_zobristMetrix->d_[i][j][d_[i][j]];

    return result;
}

int Board::step(Move &move)
{
    fall(d_,move);

    return move.score();
}

int Board::upperScore()
{
    int restScore = 0;
    int rest[NC] = {};
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            if (d_[i][j] != 0)
                rest[d_[i][j] - 1]++;

    int singleCount = 0;
    for (int i = 0;i < NC;i++)
    {
        if (rest[i] == 1)
            singleCount++;
        else
            restScore += Move::score(rest[i]);
    }

    restScore += Move::endScore(singleCount);

    return restScore;
}

void Board::rand()
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            d_[i][j] = qrand() % NC + 1;
}

Move Board::moveAt(int x, int y)
{
    Move result;
    int t[N][N];
    copy(t, d_);
    mark(t, x, y, result);

    if (result.size() == 1)
        result.clear();

    return result;
}
