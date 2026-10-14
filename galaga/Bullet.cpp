#include "Bullet.h"
#include "Enemy.h"
#include "MathUtil.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player)
    : mLocation(location), mPreviousLocation(location), mHeading(heading), mPlayer(player), mAlive(true)
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
    // Draw relative to the bullet's own location, which the engine has already applied as the offset.
    // So the tail sits at (previous - now) and the nose at the origin; together they are the world span.
    context->ScreenContext->DrawLine(mPreviousLocation - mLocation, CMPUT350::Point2D(0.0f, 0.0f), 3.0f,
                                     mPlayer ? CMPUT350::Colors::white : CMPUT350::Colors::red);
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // Only an enemy stops a bullet. The player that fired it isn't one, so the spawn overlap is ignored.
    if (std::dynamic_pointer_cast<Enemy>(obj) != nullptr) mAlive = false;
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

const CMPUT350::Rect& Bullet::GetBounds() const
{
    // Box around last frame's position and this one, so a fast bullet still hits what it passed through
    mBounds = CMPUT350::Rect(mPreviousLocation, mLocation);
    return mBounds;
}

CMPUT350::Point2D Bullet::GetLocation() const {
    return mLocation; // the nose, which is also the pivot: the bullet's shapes and drawing are relative to it
}

float Bullet::GetRotation() const {
    return 0;
}
