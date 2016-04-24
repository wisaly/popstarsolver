#include "mcts.h"
#include <QDebug>
#include <cmath>
#include <limits>
#include <algorithm>

MCTS::MCTS()
{

}

void MCTS::solve(Board *board, int simLim)
{
    map_.clear();
    breakDepth_ = 0;

    startLim_ = inspectLim_ = simLim;

    const int S = 30;
    int resourceFrame[S];
    for (int i = 0, left = simLim; i < S; i++)
    {
        resourceFrame[i] = std::max(64 * 32, left / 7);
        left -= resourceFrame[i];
    }
    int frameLim = resourceFrame[0];

    // create the root node
    MCTSNode *root = new MCTSNode(board);

    // run iterations of MCTS as long there's resources
    while (inspectLim_ > 0)
    {
        int beginInspectLim = inspectLim_;

        iterate(root, 1);

        // subtract the number used resources in the iteration from the allocated resources
        frameLim -= beginInspectLim - inspectLim_;

        // have run out of resources for this move, traverse down one level
        if (frameLim <= 0)
        {
            breakDepth_++;
            frameLim = breakDepth_ >= S ? inspectLim_ : resourceFrame[breakDepth_];
        }

        if (!(inspectLim_ & 0xf))
            qDebug() << "inspect lim:" << inspectLim_ << " break depth:" << breakDepth_;
    }

    qDebug() << "top score: " << root->topScore_;
}

bool MCTS::iterate(MCTSNode *root, int depth)
{
    if (root->isComplete())
    {
        //inspectLim_ = -1;
        return false;
    }

    // alias
    QVector<MCTSNode*> &children = root->children_;
    QList<Move> &moves = root->moves_;

    if (children.count() == 0)
    {
        qDebug() << "leaf hit @depth:" << depth;
        root->leafHit();
        //root->deactivate();
        root->complete();
        inspectLim_--;
        return false;
    }

    // account for tree-traversal cost
    if ((depth & 7) == 0)
        inspectLim_--;

    // lu-extension
    if (!root->hasLeafHit_ && inspectLim_ < startLim_ * urgencyLimit_)
        root->c_ = std::max(root->c_ * 0.9995, 8.0);

    // best index
    int bi = -1;
    // number of encountered null-children
    int nullCnt = 0;

    // find null reference and reconquering child
    for (int i = 0; i < children.count(); i++)
    {
        if (!children[i])
        {
            if (randTest(++nullCnt))
                bi = i;
        }
        else
        {
            if (!root->isOwn(i) && root->cum_ + moves[i].score() > children[i]->cum_)
            {
                if (children[i]->isComplete())
                {
                    // solved children should not be owned
                    children[i]->cum_ = root->cum_ + moves[i].score();
                    if (children[i]->cum_ + children[i]->topScore_ > topScore_)
                    {
                        // yippie, new highscore
                        topScore_ = children[i]->cum_ + children[i]->topScore_;
                        qDebug() << "top score:" << topScore_;
                        return true;
                    }
                }
                else
                {
                    children[i]->transfer(root);
                }
            }
        }
    }

    if (nullCnt == 0)
    {
        if (depth <= breakDepth_)
        {
            for (int i = 0; i < children.count(); i++)
            {
                // null-preference has higher priority than finalized choice
                if ((root->isOwn(i) && !children[i]->isComplete()) &&
                    (bi == -1 || children[i]->topScore_ > children[bi]->topScore_))
                    bi = i;
            }
            // if extremely few resources are allocated for same levels this could happen
            if (bi == -1)
            {
                breakDepth_--;
                return false;
            }
        }
        else
        {
            // choose next child base on UCB
            double lnt = std::log(root->t_);
            double buct = std::numeric_limits<double>::min();

            for (int i = 0; i < children.count(); i++)
            {
                int cum = root->cum_ + moves[i].score();
                if (root->isOwn(i) && !children[i]->isComplete())
                {
                    // ordinary case
                    double tbuct = cum + children[i]->avg() + root->c_ * std::sqrt(lnt / children[i]->t_);

                    if (tbuct > buct)
                    {
                        bi = i;
                        buct = tbuct;
                    }
                }
            }
            if (bi == -1)
            {
                // there was no suitable child
                if (!root->isAlive())
                {
                    root->leafHit();
                    root->alive_ = 0;
                    root->complete();
                }
                else
                {
                    root->deactivate();
                }

                return false;
            }
        }
    }

    // get the next node to traverse down, it could be an unexpanded child
    MCTSNode *nxt = children[bi] ? children[bi] : expandChild(root, bi);

    if (!nxt)
    {
        if (root->cnt_ == 0 && root->parent_)
            root->deactivate();
        return false;
    }

    if (nxt->t_ == 0)
    {
        if (!playout(nxt))
            return false;
    }
    else //if (nxt->t_ > 0)
    {
        if (!iterate(nxt, depth + 1))
            return false;
    }
    root->update(root->moveScore_ + nxt->topScore_);

    /*if (nxt->isComplete() && root->isOwn(bi))
    {
        // that child could have been found by somebody else and solved
        // either it already was then we should account for the leaf hit

        if (--root->alive_ == 0)
        {
            root->deactivate();
            root->complete();
        }
    }*/
    return true;
}

bool MCTS::playout(MCTSNode *leaf)
{
    // using the TabuColorRandom default policy
    if (leaf->isComplete())
        return false;

    // account for inspection of state
    inspectLim_--;

    Board board = *leaf->board_;

    // pick the tabu color
    int c[Board::NC + 1] = {};
    for (int i = 0; i < Board::N; i++)
        for (int j = 0; j < Board::N; j++)
            c[board.at(i,j)]++;

    int tabu = 1;
    int sameCount = 1;
    for (int i = 1; i <= Board::NC; i++)
    {
        if (c[i] > c[tabu])
        {
            tabu = i;
            sameCount = 1;
        }
        else if (c[i] == c[tabu] && randTest(++sameCount))
        {
            tabu = i;
        }
    }

    int blocks = Board::N * Board::N * c[0];

    int score = 0;
    for (;;)
    {
        // only use the simulation strategy if there's a significant number of blocks
        QList<Move> mvs = blocks > 48 ? board.tabuMoves(tabu) : board.moves();

        // end of game
        if (mvs.count() == 0)
        {
            score += board.endScore();
            leaf->update(score);

            return true;
        }

        // choose random move
        int mv = qrand() % mvs.count();
        // perform move
        score = board.step(mvs[mv]);
        blocks -= mvs[mv].size();
        inspectLim_--;
    }
}

MCTSNode *MCTS::expandChild(MCTSNode *root, int index)
{
    int cum = root->cum_ + root->moves_[index].score();
    Board *board = new Board(*root->board_);
    board->step(root->moves_[index]);
    long hashKey = board->hash();
    if (map_.contains(hashKey))
    {
        MCTSNode *child = map_[hashKey];
        root->children_[index] = child;
        if (cum <= child->cum_)
        {
            return nullptr;
        }
        else
        {
            child->ref_++;
            child->transfer(root);
            return child;
        }
    }
    MCTSNode *child = new MCTSNode(board, root, cum, root->moves_[index].score());
    map_.insert(hashKey, child);
    root->children_[index] = child;

    return child;
}

