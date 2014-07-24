#include <QQueue>
#include <QStack>
#include <QPair>
#include <QDebug>
#include <QElapsedTimer>
#include "board.h"

const Qt::GlobalColor clrs[6] =
{Qt::white,Qt::red,Qt::green,Qt::yellow,Qt::blue,Qt::magenta};

int scr[100];

Board::Board()
{
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            d_[i][j] = 0;

    for (int i = 0;i < 100;i++)
        scr[i] = i * i * 5;
}

Qt::GlobalColor Board::clr(int n)
{
    return clrs[n % 6];
}

void Board::rand()
{
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            d_[i][j] = qrand() % 5 + 1;
}

void Board::solve()
{
    QElapsedTimer timer;

    dbgPut(0,d_);
    timer.start();
    qDebug() << "Solve start...";

    solutionCount_ = 0;
    bestScore_ = 0;

    QList<Move> moves;
    solve(0,d_,moves);

    qDebug() << "Solve over : " << timer.elapsed() << "ms";
    qDebug() << "Best score : " << bestScore_;
    dbgPut(bestMove_ );
}

void Board::dbgPut(int c,int a[N][N])
{
    qDebug() << c << ":";
    for (int i = 0;i < N;i++)
    {
        QString s;
        for (int j = 0;j < N;j++)
            s += QString::number(a[i][j]) + " ";

        qDebug() << s;
    }
}

void Board::dbgPut(QList<Move> &moves)
{
    QString solution;
    for (int i = 0; i < moves.count();i++)
        solution += QString("(%1,%2)[%3]->")
                .arg(moves[i].n.x)
                .arg(moves[i].n.y)
                .arg(moves[i].s);

    solution = solution.left(solution.length() - 2);

    qDebug() << "Moves : " << solution;
}

void Board::fall(int c[N][N], int xfrom, int xto)
{
    for (int x = xfrom;x <= xto;x++)
    {
        for (int y = N-1;y > 0;y--)
        {
            int e = 0; // empty count
            for (int yy = y;yy >= 0 && c[yy][x] == 0;yy--)
                e++;

            if (e > 0 && y >= e)
            {
                // move down
                for (int yy = y;yy >= e;yy--)
                    c[yy][x] = c[yy - e][x];
                // fill empty
                for (int yy = e - 1;yy >= 0;yy--)
                    c[yy][x] = 0;

                y -= e - 1;
            }
        }
    }
    // fall left
    for (int x = xfrom;x <= xto;x++)
    {
        int e = 0;
        for (int xx = x; xx <= xto && c[N - 1][xx] == 0;xx++)
            e++;
        if (e > 0 && (x + e) < N)
        {
            // move left
            for (int xx = x;xx < N - e;xx++)
                for (int y = 0;y < N;y++)
                    c[y][xx] = c[y][xx + e];
            // fill empty
            for (int xx = N - e;xx < N;xx++)
                for (int y = 0;y < N;y++)
                    c[y][xx] = 0;
            x += e - 1;
        }
    }
}

void Board::mark(int c[N][N], int x, int y, QList<Node> &ss)
{
    int f = c[y][x];
    QQueue<Node> q; // search quque
    q.enqueue(Node(x,y));
    while(!q.empty())
    {
        Node h = q.dequeue();
        ss.append(h);

        c[h.y][h.x] = 0;

        if (h.x > 0 && c[h.y][h.x - 1] == f)
            q.enqueue(Node(h.x - 1,h.y));
        if (h.x < N - 1 && c[h.y][h.x + 1] == f)
            q.enqueue(Node(h.x + 1,h.y));
        if (h.y > 0 && c[h.y - 1][h.x] == f)
            q.enqueue(Node(h.x,h.y - 1));
        if (h.y < N - 1 && c[h.y + 1][h.x] == f)
            q.enqueue(Node(h.x,h.y + 1));
    }
}

void Board::solve(int score, int d[N][N], QList<Move> &moves)
{
    int c[N][N]; // temp flag map
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            c[i][j] = d[i][j];

    //dbgPut(moves.count(),c);

    QList<QList<Node> > s; // valid states
    for (int i = N - 1;i >= 0;i--)
    {
        for (int j = 0;j < N;j++)
        {
            if (c[i][j] == 0)
                continue;

            QList<Node> ss; // single state
            mark(c,j,i,ss);

            if (ss.count() >= 2)
                s.append(ss);
        }
    }

    // solved
    if (s.count() == 0)
    {
        solutionCount_ ++;
        if (solutionCount_ % 10000 == 0)
        {
            qDebug() << "Find solution "
                     << solutionCount_
                     << " score : "
                     << score
                     << " : depth " << moves.count();
        }
        //dbgPut(moves);
        //dbgPut(1,d);

        if (score > bestScore_)
        {
            bestScore_ = score;
            bestMove_ = moves;
        }

        return;
    }

    // save current
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            c[i][j] = d[i][j];
    // dfs
    for (int i = 0;i < s.count();i++)
    {
        int xfrom = N - 1;
        int xto = 0;

        // kill stars
        QList<Node> &ss = s[i];
        for (int j = 0;j < ss.count();j++)
        {
            Node &n = ss[j];
            c[n.y][n.x] = 0;

            if (n.x < xfrom)
                xfrom = n.x;
            if (n.x > xto)
                xto = n.x;
        }

        // fall down
        fall(c, xfrom, xto);

        Move m(ss[0],ss.count());
        moves.append(m);

        // recur
        solve(score + scr[ss.count()],c,moves);

        // restore
        for (int i = 0;i < N;i++)
            for (int j = 0;j < N;j++)
                c[i][j] = d[i][j];


        moves.removeLast();
    }
}
