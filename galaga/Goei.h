#ifndef GOEI_H
#define GOEI_H

#include "Enemy.h"

// The two-health ship. It takes a hit to bring down and dives on a longer, curving run.
class GoeiEnemy : public Enemy
{
public:
    GoeiEnemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath);

    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
};

#endif
