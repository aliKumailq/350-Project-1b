#include "Enemy.h"
#include "Bullet.h"

// The enemy's body is 40x40, so half of it is 20
static constexpr float kHalfSize = 20.0f;

Enemy::Enemy(CMPUT350::Point2D loc)
    : mAlive(true), mLocation(loc)
{
    // One 40x40 box centred on the enemy, in local space, so the engine places it by GetLocation
    mShapes.push_back(CMPUT350::Shape(
        CMPUT350::Rect(CMPUT350::Point2D(-kHalfSize, -kHalfSize), 2.0f * kHalfSize, 2.0f * kHalfSize)));
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

void Enemy::Update(CMPUT350::GameContext* context)
{
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    // Draw exactly the box the collisions use, so the enemy's picture and its hitbox can't disagree
    context->ScreenContext->DrawRect(GetBounds(), CMPUT350::Colors::green);
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // Only a player bullet takes an enemy out, so enemy bullets would pass through their own kind
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && bullet->IsPlayerBullet()) mAlive = false;
}

void Enemy::Kill()
{
    mAlive = false;
}

bool Enemy::IsAlive() const
{
    return mAlive;
}

const std::vector<CMPUT350::Shape>& Enemy::GetShapes()
{
    return mShapes; // local, so it is built once and handed back unchanged
}

const CMPUT350::Rect& Enemy::GetBounds() const
{
    // Rebuilt from the centre, so the box follows if the enemy later flies out on an attack
    mBounds = CMPUT350::Rect(mLocation - CMPUT350::Point2D(kHalfSize, kHalfSize), 2.0f * kHalfSize, 2.0f * kHalfSize);
    return mBounds;
}

CMPUT350::Point2D Enemy::GetLocation() const {
    return mLocation; // the centre, which is also the pivot the engine rotates shapes around
}

float Enemy::GetRotation() const {
    return 0;
}
