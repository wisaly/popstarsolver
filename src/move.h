#ifndef MOVE_H
#define MOVE_H

#include <QList>

class Move
{
public:
    class Node
    {
    public:
        Node(int x, int y) : x_(x), y_(y) {}
        int x_ = 0;     // x pos
        int y_ = 0;     // y pos
    };

    QList<Node> cell_;          // cell in same color
    int color_ = 0;
    bool isVS_ = false;

    int score() const;          // move score
    inline int size() const { return cell_.size(); }

    static int score(int s);
};

#endif // MOVE_H