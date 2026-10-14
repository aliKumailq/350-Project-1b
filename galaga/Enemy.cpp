#include "Enemy.h"

#include <cmath>
#include <memory>

#include "Bullet.h"

// The enemy's body is 40x40, so half of it is 20
static constexpr float kHalfSize = 20.0f;
// How much of the curve a frame walks. A fixed step is what keeps the trip at a constant speed instead of
// easing in and out with the control points.
static constexpr float kPathStep = 0.02f;
// How far and how fast a settled ship drifts in its slot
static constexpr float kHoverAmplitude = 18.0f;
static constexpr float kHoverRate = 0.06f;
// How fast an enemy shot drops, in pixels per frame (down the screen, so y grows)
static constexpr float kEnemyBulletSpeed = 10.0f;

Enemy::Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath, int health, int points)
    : mHealth(health), mMaxHealth(health), mPoints(points), mAlive(true),
      mLocation(loc), mRotation(0.0f), mFormation(loc),
      mPhase(Phase::kFormation), mT(0.0f), mMaxT(0.0f), mFiredThisDive(false), mNotified(false)
{
    // One 40x40 box centred on the enemy, in local space, so the engine places it by GetLocation
    mShapes.push_back(CMPUT350::Shape(
        CMPUT350::Rect(CMPUT350::Point2D(-kHalfSize, -kHalfSize), 2.0f * kHalfSize, 2.0f * kHalfSize)));

    // A real entry path is at least one cubic (four points). Anything shorter has no curve to walk, so the
    // ship just starts settled in its slot rather than tripping the bezier guard.
    if (entryPath.size() > 3)
    {
        mPath = entryPath;
        mMaxT = float((mPath.size() - 1) / 3);
        mPhase = Phase::kEntering;
        PlaceOnCurve(0.0f); // start at the far end of the path, off screen
    }
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    switch (mPhase)
    {
    case Phase::kEntering:
        mT += kPathStep;
        if (mT >= mMaxT) { mT = mMaxT; mPhase = Phase::kFormation; }
        PlaceOnCurve(mT);
        break;

    case Phase::kFormation:
        HoverInFormation(context);
        break;

    case Phase::kAttacking:
        mT += kPathStep;
        // Fire once, around the middle of the dive, so the shot lands while the ship is away from its slot
        if (!mFiredThisDive && mT >= 0.5f * mMaxT) { mFiredThisDive = true; Fire(context); }
        if (mT >= mMaxT) { mT = mMaxT; mPhase = Phase::kReturning; }
        PlaceOnCurve(mT);
        break;

    case Phase::kReturning:
        mT -= kPathStep;
        if (mT <= 0.0f) { mT = 0.0f; mPhase = Phase::kFormation; }
        PlaceOnCurve(mT);
        break;
    }
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    // A ship that died this frame tells the game once. CollisionEnter has no context, so the message waits
    // for here, which still runs the same frame, before the engine clears the dead ship out.
    if (!mAlive && !mNotified)
    {
        mNotified = true;
        context->mNotificationManager->Notify(kEnemyDestroyed);
    }
}

bool Enemy::IsAlive() const
{
    return mAlive;
}

void Enemy::Kill()
{
    mAlive = false;
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

float Enemy::GetRotation() const
{
    return mRotation;
}

CMPUT350::Point2D Enemy::GetLocation() const
{
    return mLocation; // the centre, which is also the pivot the engine rotates shapes around
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // The only things an enemy can meet are player bullets and the player itself. An enemy bullet is ignored,
    // so it passes through its own kind.
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr)
    {
        if (!bullet->IsPlayerBullet()) return;

        mHealth--;
        if (mHealth <= 0) mAlive = false;
        return;
    }

    // Anything that is not a bullet is the player, and flying into the player destroys the enemy
    mAlive = false;
}

const std::vector<CMPUT350::Shape>& Enemy::GetShapes()
{
    return mShapes; // local, so it is built once and handed back unchanged
}

const CMPUT350::Rect& Enemy::GetBounds() const
{
    // Rebuilt from the centre, so the box follows the ship through its entry and dives
    mBounds = CMPUT350::Rect(mLocation - CMPUT350::Point2D(kHalfSize, kHalfSize), 2.0f * kHalfSize, 2.0f * kHalfSize);
    return mBounds;
}

bool Enemy::InFormation() const
{
    return mPhase == Phase::kFormation;
}

int Enemy::Points() const
{
    // A ship caught away from its slot is worth double, so a kill mid-dive scores more than a kill in the grid
    return InFormation() ? mPoints : (2 * mPoints);
}

bool Enemy::BeginAttack(const std::vector<CMPUT350::Point2D>& attackPath)
{
    // Only a settled ship may dive; one still flying in, already diving, or on its way back is busy
    if (mPhase != Phase::kFormation) return false;
    if (attackPath.size() < 4) return false; // nothing to walk without at least one cubic

    mPath = attackPath;
    mPath[0] = mLocation; // join on smoothly by starting the dive exactly where the ship is
    mMaxT = float((mPath.size() - 1) / 3);
    mT = 0.0f;
    mFiredThisDive = false;
    mPhase = Phase::kAttacking;
    return true;
}

void Enemy::DrawBody(CMPUT350::GameContext* context, CMPUT350::RGBColor base) const
{
    const CMPUT350::RGBColor colour = HealthColor(base);

    // The box the collisions use, plus a nose pointing the way the ship faces, so its rotation shows
    context->ScreenContext->DrawRect(
        CMPUT350::Rect(CMPUT350::Point2D(-kHalfSize, -kHalfSize), 2.0f * kHalfSize, 2.0f * kHalfSize), colour);
    context->ScreenContext->DrawRect(
        CMPUT350::Rect(CMPUT350::Point2D(-0.35f * kHalfSize, -1.1f * kHalfSize), 0.7f * kHalfSize, 0.5f * kHalfSize),
        colour);
}

CMPUT350::RGBColor Enemy::HealthColor(CMPUT350::RGBColor base) const
{
    // Full colour at full health, darker as it drops, so a damaged ship reads as weakened
    const float shade = 0.45f + 0.55f * (float(mHealth) / float(mMaxHealth));
    return CMPUT350::RGBColor(uint8_t(base.r * shade), uint8_t(base.g * shade), uint8_t(base.b * shade));
}

void Enemy::PlaceOnCurve(float t)
{
    mLocation = Bezier::GetPoint(mPath, t);

    // Face the way the curve is heading. The slope points along the travel, and the quarter turn makes
    // "straight up" read as zero, which is how the ship is drawn. On the way back the ship faces the other way.
    CMPUT350::Point2D slope = Bezier::GetSlope(mPath, t);
    if (mPhase == Phase::kReturning) slope = slope * -1.0f;
    mRotation = atan2f(slope.y, slope.x) + 1.5707963f;
}

void Enemy::HoverInFormation(CMPUT350::GameContext* context)
{
    // sin of the shared frame clock, so the whole formation drifts as one instead of each ship on its own
    const float drift = sinf(float(context->currentFrame) * kHoverRate) * kHoverAmplitude;
    mLocation = CMPUT350::Point2D(mFormation.x + drift, mFormation.y);
    mRotation = 0.0f;
}

void Enemy::Fire(CMPUT350::GameContext* context)
{
    // A straight drop from the ship's belly, the way a diving ship shoots as it comes down. The enemy is
    // named as the parent, so the shot disappears with the ship that fired it.
    auto shot = std::make_shared<Bullet>(mLocation + CMPUT350::Point2D(0.0f, kHalfSize),
                                         CMPUT350::Point2D(0.0f, kEnemyBulletSpeed), false, context->CurrObject);
    context->mEngineView->AddGameObject(shot);
}
