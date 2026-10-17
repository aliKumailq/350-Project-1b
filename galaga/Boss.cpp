#include "Boss.h"


static constexpr float kWidth = 120.0f;
static constexpr float kHalfWidth = kWidth / 2.0f;

static constexpr float kHeight = 40.0f;
static constexpr float kHalfHeight = kHeight / 2.0f;



// Three health, a hundred and fifty points a kill
BossEnemy::BossEnemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath)
    : Enemy(loc, entryPath, 3, 150)
{
  mShapes.push_back(CMPUT350::Rect(CMPUT350::Point2D(-40.0f, -12.0f), 80.0f, 24.0f));
}

bool BossEnemy::Attack(CMPUT350::GameContext*, const CMPUT350::Point2D& target)
{
    // A steep drop almost straight at the player, then a hook to the side to peel away
    const CMPUT350::Point2D start = GetLocation();
    const std::vector<CMPUT350::Point2D> path = {
        start,
        CMPUT350::Point2D((start.x + target.x) * 0.5f, start.y + 320.0f),
        CMPUT350::Point2D(target.x - 220.0f, target.y - 300.0f),
        CMPUT350::Point2D(target.x, target.y - 120.0f),
    };
    return BeginAttack(path);
}

void BossEnemy::RenderForeground(CMPUT350::GameContext* context)
{
    // Wide side fins under a magenta body, so the flagship looks heavier than the smaller ships
    const CMPUT350::RGBColor colour = HealthColor(CMPUT350::Colors::magenta);
    context->ScreenContext->DrawRect(CMPUT350::Rect(CMPUT350::Point2D(-40.0f, -12.0f), 80.0f, 24.0f), colour);
    DrawBody(context, CMPUT350::Colors::magenta);
}
const CMPUT350::Rect& BossEnemy::GetBounds() {
    // Rebuilt from the centre, so the box follows the ship through its entry and dives
    mBounds = CMPUT350::Rect(CMPUT350::Point2D(0,0) - CMPUT350::Point2D(kHalfWidth, kHalfHeight), kWidth, kHeight);
    // Remember that all internal locations must be relative to the collision objects origin.
    return mBounds;
}
