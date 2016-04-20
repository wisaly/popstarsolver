#ifndef MCTSNode_H
#define MCTSNode_H

#include "board.h"
#include "move.h"
#include "solution.h"
#include <QVector>

class MCTSNode
{
public:

    MCTSNode *parent_ = nullptr;        // parent node
    QVector<MCTSNode*> children_;       // child nodes

    Board *board_;                      // the board
    QList<Move> moves_;                 // available moves (with score), save solution when complete solved
    int cum_ = 0;                       // best cumulative score leading to this state (include moveScore)
    int topScore_ = -1;                 // best score from this node
    int moveScore_ = 0;                 // score perform to this node
    int upperScore_ = 0;                // an upper bound on the maximum possible score achievable
    int t_ = -1;                        // number of visits
    int cnt_ = 0;                       // number of active child nodes
    int alive_ = 0;                     // number of unsolved child nodes
    bool hasLeafHit_ = false;           // is there a terminal node in this subtree
    double c_ = 0.0;                    // explorative factor
    int ref_ = 1;                       // reference count

private:
    bool complete_ = false;             // is node complete solved
    double avg_ = .0f;                  // average score
public:
    MCTSNode();
    ~MCTSNode();
    MCTSNode(Board *board, MCTSNode *parent = nullptr, int cum = 0, int moveScore = 0);

    // records an iteration yielding the given score in this node
    void update(int sample);
    // revokes this node's ownership of the given child node
    void deactivate();
    // change parent
    void transfer(MCTSNode *newParent);
    // return whether there are unsolved child nodes of this node
    bool isAlive();
    // record that a terminal node is present in all ancestors of the given terminal node, and increases their c value
    void leafHit();
    // is own child at index
    bool isOwn(int index);
    // mark complete and save solution
    void complete();
    // free children resources
    void freeChildren();
    // attributes accessor
    bool isComplete() { return complete_; }
    double avg() { return avg_; }
};

#endif // MCTSNode_H
