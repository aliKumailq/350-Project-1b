#ifndef ZAKO_H
#define ZAKO_H

#include "Enemy.h"

// The common soldier: one hit kills it, and it dives in a single wide swoop.
class ZakoEnemy : public Enemy
{
public:
    ZakoEnemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath);

    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
};

#endif
