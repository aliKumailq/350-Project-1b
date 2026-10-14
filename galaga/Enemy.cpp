#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(CMPUT350::Point2D loc)
    : mAlive(true), mLocation(loc)
{
    // Make a 40×40 rectangle whose center is at the given loc
    mBounds = CMPUT350::Rect(mLocation - CMPUT350::Point2D(20.0f, 20.0f), 40.0f, 40.0f);
    mShapes = {};
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

bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return (false);
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    // Draw exactly the box the collisions use, so the enemy's picture and its hitbox can't disagree
    auto boundDraw = mBounds;
    boundDraw.topLeft -= mLocation;
    context->ScreenContext->DrawRect(boundDraw, CMPUT350::Colors::green);
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

const CMPUT350::Rect& Enemy::GetBounds()
{
    return mBounds;
}


const std::vector<CMPUT350::Shape>& Enemy::GetShapes()  {

    return mShapes;


}

CMPUT350::Point2D Enemy::GetLocation() const {
    return mLocation;
}

float Enemy::GetRotation() const {
    return 0;
}