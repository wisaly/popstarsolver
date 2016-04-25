#include "mctsnode.h"

MCTSNode::MCTSNode()
{

}

MCTSNode::~MCTSNode()
{
    delete board_;

    freeChildren();
}

MCTSNode::MCTSNode(Board *board, MCTSNode *parent, int cum, int moveScore)
{
    board_ = board;
    moves_ = board->moves();
    //complete_ = moves_.isEmpty();

    upperScore_ = board->upperScore();
    alive_ = cnt_ = moves_.count();
    children_.resize(cnt_);

    parent_ = parent;
    cum_ = cum;
    moveScore_ = moveScore;

    //c_ = defaultC*1.00 + rnd.nextDouble()*defaultC*0.00;
}

void MCTSNode::update(int sample)
{
    //#if (solved_)return;
    t_++;
    Q_ASSERT(t_ > 0);
    avg_ += (sample - avg_) / t_;
    if (sample > topScore_)
        topScore_ = sample;
}

void MCTSNode::deactivate()
{
    if (!isComplete())
    {
        // subtract the statistics of node kid from this node and all its ancestors
        int t = t_;
        double sub = avg_;
        for (MCTSNode *p = this; p->parent_; p = p->parent_)
        {
            MCTSNode *pp = p->parent_;
            sub += p->moveScore_;
            Q_ASSERT(pp->t_ > t);
            pp->avg_ = (pp->avg_ * pp->t_ - sub * t) / (pp->t_ - t);
            pp->t_ -= t;
        }
    }

    if (parent_ != 0 && --parent_->cnt_ <= 0)
        parent_->deactivate();

    parent_ = nullptr;
}

void MCTSNode::transfer(MCTSNode *newParent)
{
    if (newParent->parent_ == this)
        Q_ASSERT(newParent->parent_ != this);
    cum_ = newParent->cum_ + moveScore_;
    deactivate();
    parent_ = newParent;
    newParent->cnt_++;

    if (!isComplete())
    {
        // add the statistics of node kid to this node and all its ancestors
        int t = t_;
        Q_ASSERT(t > 0);
        double ad = avg_;
        for (MCTSNode *p = this; p->parent_; p = p->parent_)
        {
            MCTSNode *pp = p->parent_;
            ad += p->moveScore_;
            Q_ASSERT(pp->t_ > 0 || t > 0);
            pp->avg_ = (pp->avg_ * pp->t_ + ad * t_) / (pp->t_ + t);
            pp->t_ += t;
        }
    }
}

bool MCTSNode::isAlive()
{
    for (MCTSNode *child : children_)
        if (!child || child->t_ >= 0)
            return true;

    return false;
}

void MCTSNode::leafHit()
{
    double mul = 1.05;
    for (MCTSNode *leaf = this; leaf; leaf = leaf->parent_)
    {
        leaf->c_ *= mul;
        leaf->hasLeafHit_ = true;
        mul = qMax(mul * 0.99, 1.001);
    }
}

bool MCTSNode::isOwn(int index)
{
    return children_[index]->parent_ == this;
}

void MCTSNode::complete()
{
    QList<Move> bestMoves;
    MCTSNode *p = this;
    for (;p ;)
    {
        int bi = -1;
        int topScore = 0;
        for (int i = 0; i < p->children_.count(); i++)
        {
            if (p->children_[i] && (bi == -1 || p->children_[i]->topScore_ > topScore))
            {
                bi = i;
                topScore = p->children_[i]->topScore_;
            }
        }

        if (bi == -1)
            break;  //# this may not happen

        bestMoves.append(p->moves_[bi]);

        p = p->children_[bi];
        if (p && p->isComplete())
        {
            bestMoves.append(p->moves_);
            break;
        }
    }
    moves_ = bestMoves;

    //freeChildren();
    deactivate();
    complete_ = true;

    if (parent_)
    {
        parent_->alive_--;
        if (parent_->alive_ <= 0)
            parent_->complete();
    }
}

void MCTSNode::freeChildren()
{
    for (int i = 0; i < children_.count(); i++)
    {
        if (children_[i])
        {
            children_[i]->ref_--;
            if (isOwn(i) && children_[i]->ref_ < 1)
            {
                delete children_[i];
                children_[i] = nullptr;
            }
        }
    }
}

