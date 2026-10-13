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

bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return (false); // This expects a bool in return
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    // Draw the same span the collision box covers, so what you see is what can be hit
    context->ScreenContext->DrawLine(mPreviousLocation, mLocation, 3.0f,
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

const CMPUT350::Rect& Bullet::GetBounds()
{
    // Box around last frame's position and this one, so a fast bullet still hits what it passed through
    mBounds = CMPUT350::Rect(mPreviousLocation, mLocation);
    return mBounds;
}

CMPUT350::Point2D Bullet::GetLocation() const {
    return mLocation;
}

float Bullet::GetRotation() const {
    return 0;
}
