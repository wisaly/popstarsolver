#include "mctsnode.h"

MCTSNode::MCTSNode()
{

}

MCTSNode::~MCTSNode()
{
    delete board_;

    for (int i = 0; i < children_.count(); i++)
        if (children_[i] && isOwn(i))
            delete children_[i];
}

MCTSNode::MCTSNode(Board *board, MCTSNode *parent, int cum, int moveScore)
{
    board_ = board;
    moves_ = board->moves();
    //complete_ = moves_.isEmpty();

    upperScore_ = board->upperScore();
    alive_ = cnt_ = moves_.count() / 2;
    children_.resize(cnt_);

    parent_ = parent;
    cum_ = cum;
    moveScore_ = moveScore;
}

void MCTSNode::update(int sample)
{
    //#if (solved_)return;
    t_++;
    avg_ += (sample - avg_) / t_;
    if (sample > topScore_)
        topScore_ = sample;
}

void MCTSNode::deactivateChild(MCTSNode *kid)
{
    if (kid->parent_ == this)
    {
        subtract(kid);
        if (--cnt_ == 0 && parent_ != 0)
            parent_->deactivateChild(this);
    }
}

void MCTSNode::activateChild(MCTSNode *kid, int cum)
{
    kid->cum_ = cum;
    if (kid->parent_ != this)
    {
        kid->parent_->deactivateChild(kid);
        kid->parent_ = this;
        cnt_++;
        add(kid);
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
    MCTSNode *leaf = this;
    for (double mul = 1.05; leaf; mul = qMax(mul * 0.99, 1.001), leaf = leaf->parent_)
    {
        leaf->c_ *= mul;
        leaf->hasLeafHit_ = true;
    }
}

void MCTSNode::subtract(MCTSNode *kid)
{
    int t = kid->t_;
    if (t < 0)
        return;

    double sub = kid->avg_;
    for (MCTSNode *p = this; p; kid = p, p = p->parent_)
    {
        sub += p->moveScore_;
        p->avg_ = (p->avg_ * p->t_ - sub * t_) / (p->t_ - t);
        p->t_ -= t;
    }
}

void MCTSNode::add(MCTSNode *kid)
{
    int t = kid->t_;
    if (t < 0)
        return;

    double ad = kid->avg_;
    for (MCTSNode *p = this; p; kid = p, p = p->parent_)
    {
        ad += p->moveScore_;
        p->avg_ = (p->avg_ * p->t_ + ad * t_) / (p->t_ + t);
        p->t_ += t;
    }
}

bool MCTSNode::isOwn(int index)
{
    return children_[index]->parent_ == this;
}

Solution MCTSNode::solution()
{
    Solution result;
    MCTSNode *p = this;
    for (;;)
    {
        int bi = -1;
        int topScore = 0;
        for (int i = 0; i < children_.count(); i++)
        {
            if (children_[i] && (bi == -1 || children_[i]->topScore_ > topScore))
            {
                bi = i;
                topScore = children_[i]->topScore_;
            }
        }

        if (bi == -1)
            break;

        result.moves_.append(p->moves_[bi]);
        result.score_ += p->children_[bi]->moveScore_;
    }

    return result;
}

