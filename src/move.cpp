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
    }

    int score(int size)
    {
        return d_[size];
    }
private:
    int d_[N];
};

Q_GLOBAL_STATIC(ScoreCache, g_scoreCache)

int Move::score() const
{
    return score(size());
}

int Move::score(int s)
{
    return g_scoreCache->score(s);
}
