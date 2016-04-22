#ifndef MCTS_H
#define MCTS_H

/* Mento-Carlo Tree Search
 *
 * part of PopStarSolver
 * reference to https://github.com/bwakell/MCTS_SameGame
 */

#include "board.h"
#include "move.h"
#include "solution.h"
#include "mctsnode.h"
#include <QHash>

class MCTS
{
public:
    MCTS();

    void solve(Board *board, int simLim);
private:
    bool iterate(MCTSNode *root, int depth);
    // generate a float random number between 0 and 1
    //inline static double rand() { return static_cast<double>(qrand()) / RAND_MAX; }
    // test a random number with decreasing chance
    inline static bool randTest(int exponent) { return exponent == 1 ? true : qrand() % exponent == 0; }
    // run a simulation from the node leaf at given depth, the length of the solution is returned
    bool playout(MCTSNode *leaf);
    // return child of node root that is reached by applying move at moveIndex
    MCTSNode *expandChild(MCTSNode *root, int index);
private:
    double defaultC_ = 0.021 * 5000;
    double urgencyLimit_ = 0.3;
    int inspectLim_ = 0;
    int startLim_ = 0;
    int breakDepth_ = 0;
    QHash<long,MCTSNode*> map_;
    int topScore_ = 0;                  // best score
};

#endif // MCTS_H
