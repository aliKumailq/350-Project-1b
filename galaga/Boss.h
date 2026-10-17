#ifndef BOSS_H
#define BOSS_H

#include "Enemy.h"

// The flagship: three health, worth the most, and drawn heavier than the others.
class BossEnemy : public Enemy
{
public:
    BossEnemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath);

    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    const CMPUT350::Rect& GetBounds() override;
};

#endif
