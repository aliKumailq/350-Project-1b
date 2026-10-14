#include "Goei.h"

// Two health, eighty points a kill
GoeiEnemy::GoeiEnemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath)
    : Enemy(loc, entryPath, 2, 80)
{
}

bool GoeiEnemy::Attack(CMPUT350::GameContext*, const CMPUT350::Point2D& target)
{
    // An S-run built from two cubics (seven points, the middle one shared): it swings right, curls back
    // left, then drops onto the player from the side
    const CMPUT350::Point2D start = GetLocation();
    const std::vector<CMPUT350::Point2D> path = {
        start,
        CMPUT350::Point2D(start.x + 170.0f, start.y + 160.0f),
        CMPUT350::Point2D(start.x + 60.0f, start.y + 420.0f),
        CMPUT350::Point2D(target.x + 90.0f, target.y - 330.0f), // the join between the two cubics
        CMPUT350::Point2D(target.x - 150.0f, target.y - 210.0f),
        CMPUT350::Point2D(target.x + 40.0f, target.y - 130.0f),
        CMPUT350::Point2D(target.x, target.y - 100.0f),
    };
    return BeginAttack(path);
}

void GoeiEnemy::RenderForeground(CMPUT350::GameContext* context)
{
    // Yellow body with a bright dot in the middle, so it reads as a different ship from the green soldier
    const CMPUT350::RGBColor colour = HealthColor(CMPUT350::Colors::yellow);
    DrawBody(context, CMPUT350::Colors::yellow);
    context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.0f, 0.0f), 7.0f, colour);
}
