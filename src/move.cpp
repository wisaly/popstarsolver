#include "move.h"

// generate scores in memory
class ScoreCache
{
    enum { N = 100 };
public:
    ScoreCache()
    {
        for (int i = 0; i < N; i++)
            d_[i] = i * i * 5;

        for (int i = 0; i < 10; i++)
            e_[i] = 2000 - i * i * 20;
    }

    int score(int size) { Q_ASSERT(size < N); return d_[size]; }
    int endScore(int size) { return size >= 10 ? 0 : e_[size]; }

private:
    int d_[N];
    int e_[10];
};

Q_GLOBAL_STATIC(ScoreCache, g_scoreCache)

int Move::score() const
{
    return score(size());
}

int Move::score(int n)
{
    return g_scoreCache->score(n);
}

int Move::endScore(int n)
{
    return g_scoreCache->endScore(n);
}

bool Move::isIn(int x, int y)
{
    for (const Node &node : cell_)
        if (node.x_ == x && node.y_ == y)
            return true;

    return false;
}

void Move::clear()
{
    cell_.clear();
    color_ = 0;
    isVS_ = false;
}
