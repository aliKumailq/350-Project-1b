#include "Bullet.h"
#include "Enemy.h"
#include "MathUtil.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player,
               std::weak_ptr<CMPUT350::GameObject> parent)
    : mLocation(location), mPreviousLocation(location), mHeading(heading), mPlayer(player), mAlive(true),
      mParent(parent), mCollisionPoint(0.0f, 0.0f), mHit(false)
{
}

bool Bullet::IsPlayerBullet()
{
    return mPlayer;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

void Bullet::Update(CMPUT350::GameContext* context)
{
    // If whoever fired us is gone, we go too, so a dead ship never leaves a shot in flight
    if (mParent.expired()) mAlive = false;

    // Save where you were, then move foreward
    mPreviousLocation = mLocation;
    mLocation += mHeading;

    // Leaving the screens top or bottom finishes the bullet. One that never died would hold a player bullet slot forever.
    if (mLocation.y < 0 || mLocation.y > context->ScreenContext->GetWindowHeight()) mAlive = false;
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    const CMPUT350::RGBColor colour = mPlayer ? CMPUT350::Colors::white : CMPUT350::Colors::red;

    if (mHit)
    {
        // Once it has struck, the bullet is only a small mark at the impact point. The point is in world
        // space, so subtract our own location to place it in the local frame the engine offsets for us.
        context->ScreenContext->DrawCircle(mCollisionPoint, 8.0f, colour);
        return;
    }

    // Draw relative to the bullet's own location, which the engine has already applied as the offset.
    // So the tail sits at (previous - now) and the nose at the origin; together they are the world span.
    context->ScreenContext->DrawLine(mPreviousLocation - mLocation, CMPUT350::Point2D(0.0f, 0.0f), 3.0f, colour);
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // Only an enemy stops a bullet. The player that fired it isn't one, so the spawn overlap is ignored.
    if (std::dynamic_pointer_cast<Enemy>(obj) != nullptr) mAlive = false;
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                            const CMPUT350::Point2D& collisionPoint)
{
    // The engine tells us where we struck. If several enemies are hit on the same frame, keep the point
    // furthest down the screen (largest y), which is where the bullet would have got to last.
    if (!mHit || collisionPoint.y > mCollisionPoint.y)
    {
        mCollisionPoint = collisionPoint;
        mHit = true;
    }

    CollisionEnter(obj); // the same "did this stop us" test as the point-free version
}

void Bullet::Kill()
{
    mAlive = false;
}

bool Bullet::IsAlive() const
{
    return mAlive;
}

const std::vector<CMPUT350::Shape>& Bullet::GetShapes()
{
    // The bullet's location is its nose, so in local space the swept path runs from where it was (relative
    // to where it is now) back to the origin. Offsetting by GetLocation turns it into the world segment.
    mShapes.clear();
    mShapes.push_back(CMPUT350::Shape(CMPUT350::Line(mPreviousLocation - mLocation, CMPUT350::Point2D(0.0f, 0.0f))));
    return mShapes;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    // Bounds are in local space now, so the box has to cover the bullet's own shape, not sit on its
    // nose. The swept path runs from where it was (relative to the nose) back to the origin, and
    // Rect(p1, p2) builds the positive-size box around both points.
    mBounds = CMPUT350::Rect(mPreviousLocation - mLocation, CMPUT350::Point2D(0.0f, 0.0f));
    return mBounds;
}

CMPUT350::Point2D Bullet::GetLocation() const {
    return mLocation; // the nose, which is also the pivot: the bullet's shapes and drawing are relative to it
}

float Bullet::GetRotation() const {
    return 0;
}
