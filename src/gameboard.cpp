#include <QQueue>
#include <QStack>
#include <QPair>
#include <QDebug>
#include <QElapsedTimer>
#include "gameboard.h"

const Qt::GlobalColor clrs[6] =
{Qt::white,Qt::red,Qt::green,Qt::yellow,Qt::blue,Qt::magenta};

int scr[100];

GameBoard::GameBoard()
{
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            d_[i][j] = 0;

    for (int i = 0;i < 100;i++)
        scr[i] = i * i * 5;

    qsrand(811);
}

Qt::GlobalColor GameBoard::color(int n)
{
    return clrs[n % 6];
}

void GameBoard::rand()
{
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            d_[i][j] = qrand() % M + 1;
}

void GameBoard::select(int x, int y)
{
    QList<Node> s;
    mark(d_,x,y,s);
    fall(d_,0,N-1);
}


void GameBoard::dbgPut(int c,int a[N][N])
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

void GameBoard::dbgPut(QList<Move> &moves)
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

void GameBoard::fall(int c[N][N], int xfrom, int xto)
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

void GameBoard::mark(int c[N][N], int x, int y, QList<Node> &ss)
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

void GameBoard::solve()
{
    QElapsedTimer timer;

    dbgPut(0,d_);
    timer.start();
    qDebug() << "Solve start...";

    solutionCount_ = 0;
    bestScore_ = 0;
    searchHistory_.clear();

    QList<Move> moves;
    solve(0,d_,moves);

    qDebug() << "Solve over : " << timer.elapsed() << "ms";
    qDebug() << "Best score : " << bestScore_;
    dbgPut(bestMove_ );
}

void GameBoard::averageCells()
{
    const int rounds = 10000;
    int totalCells = 0;
    for (int i = 0;i < rounds;i++)
    {
        rand();
        QList<QList<Node> > s; // valid states
        for (int i = N - 1;i >= 0;i--)
        {
            for (int j = 0;j < N;j++)
            {
                if (d_[i][j] == 0)
                    continue;

                QList<Node> ss; // single state
                mark(d_,j,i,ss);

                if (ss.count() >= 2)
                    s.append(ss);
            }
        }
        totalCells += s.count();
    }
    qDebug() << "avage cells:" << (double)totalCells / rounds;
}

int GameBoard::solve(int score, int d[N][N], QList<Move> &moves)
{
    int c[N][N]; // temp flag map
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            c[i][j] = d[i][j];

    QByteArray currentState = stateSign(d);
    if (searchHistory_.contains(currentState))
        if (score + searchHistory_[currentState] <= bestScore_)
            return 0;

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
        //dbgPut(moves);
        //dbgPut(1,d);

        if (score > bestScore_)
        {
            bestScore_ = score;
            bestMove_ = moves;

            qDebug() << "Find solution "
                     << solutionCount_
                     << " best score : "
                     << bestScore_
                     << " : depth " << moves.count();
        }

        return score;
    }

    // save current
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            c[i][j] = d[i][j];

    int currentBest = 0;
    // dfs
    for (int i = 0;i < s.count();i++)
    {
        if (!worthSearch(score,d))
        {
            return 0;
        }

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
        int solveScore = solve(score + scr[ss.count()],c,moves);

        if (solveScore > currentBest)
            currentBest = solveScore;

        // restore
        for (int i = 0;i < N;i++)
            for (int j = 0;j < N;j++)
                c[i][j] = d[i][j];

        moves.removeLast();
    }

    searchHistory_.insert(currentState,currentBest - score);

    return currentBest;
}

bool GameBoard::worthSearch(int score, int c[N][N])
{
    int rest[M]={};
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            if (c[i][j] != 0)
                rest[c[i][j] - 1]++;

    int restScore = 0;
    for (int i = 0;i < M;i++)
        restScore += scr[rest[i]];

    return score + restScore > bestScore_;
}

QByteArray GameBoard::stateSign(int c[N][N])
{
    QByteArray result(N*N,0);
    int k = 0;
    for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
            result[k++]=c[i][j];

    return result;
}
