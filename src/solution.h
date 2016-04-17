#ifndef SOLUTION_H
#define SOLUTION_H

#include "move.h"
#include <QVector>

class Solution
{
public:
    Solution();

    int score_ = 0;
    QVector<Move> moves_;

};

#endif // SOLUTION_H
