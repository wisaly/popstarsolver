#ifndef MCTSNode_H
#define MCTSNode_H

#include "board.h"
#include "move.h"
#include "solution.h"
#include <QVector>

class MCTSNode
{
public:
    MCTSNode();
    ~MCTSNode();

    MCTSNode *parent_ = nullptr;        // parent node
    QVector<MCTSNode*> children_;       // child nodes

    Board *board_;                      // the board
    QList<Move> moves_;                 // available moves (with score)
    int cum_ = 0;                       // best cumulative score leading to this state (include moveScore)
    int topScore_ = -1;                 // best score from this node
    int moveScore_ = 0;                 // score perform to this node
    int upperScore_ = 0;                // an upper bound on the maximum possible score achievable
    int t_ = -1;                        // number of visits
    int cnt_ = 0;                       // number of active child nodes
    int alive_ = 0;                     // number of unsolved child nodes
    bool hasLeafHit_ = false;           // is there a terminal node in this subtree
    double avg_ = .0f;                  // average score
    double c_ = 0.0;                    // explorative factor
    bool complete_ = false;             // is node complete solved

    MCTSNode(Board *board, MCTSNode *parent = nullptr, int cum = 0, int moveScore = 0);

    // records an iteration yielding the given score in this node
    void update(int sample);
    // revokes this node's ownership of the given child node
    void deactivateChild(MCTSNode *kid);
    // make this node the owner of child reached using the given cumulative score (cum includes score[i])
    void activateChild(MCTSNode *kid, int cum);
    // return whether there are unsolved child nodes of this node
    bool isAlive();
    // record that a terminal node is present in all ancestors of the given terminal node, and increases their c value
    void leafHit();
    // subtract the statistics of node kid from this node and all its ancestors
    void subtract(MCTSNode *kid);
    // add the statistics of node kid to this node and all its ancestors
    void add(MCTSNode *kid);
    // is own child at index
    bool isOwn(int index);
    Solution solution();
};

#endif // MCTSNode_H
