#include "Zako.h"

// One health, fifty points a kill
ZakoEnemy::ZakoEnemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath)
    : Enemy(loc, entryPath, 1, 50)
{
}

bool ZakoEnemy::Attack(CMPUT350::GameContext*, const CMPUT350::Point2D& target)
{
    // A wide swoop out to the left, down past the player, then back up to end the run just above them
    const CMPUT350::Point2D start = GetLocation();
    const std::vector<CMPUT350::Point2D> path = {
        start,
        CMPUT350::Point2D(start.x - 150.0f, start.y + 220.0f),
        CMPUT350::Point2D(target.x + 140.0f, target.y - 250.0f),
        CMPUT350::Point2D(target.x, target.y - 110.0f),
    };
    return BeginAttack(path);
}

void ZakoEnemy::RenderForeground(CMPUT350::GameContext* context)
{
    DrawBody(context, CMPUT350::Colors::green);
}
