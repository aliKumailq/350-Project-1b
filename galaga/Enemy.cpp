#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(CMPUT350::Point2D loc)
    : mAlive(true), mShapes{CMPUT350::Rect(loc - CMPUT350::Point2D(20.0f, 20.0f), 40.0f, 40.0f)}
{

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
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::green);
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
    return mShapes[0].shape.rect; // the enemy never moves, so its box is fixed from construction
}

CMPUT350::Point2D Enemy::GetLocation() const {
    return CMPUT350::Point2D(0,0);
}

float Enemy::GetRotation() const {
    return 0;
}


const std::vector<CMPUT350::Shape> &Enemy::GetShapes() const {
    return mShapes;
}
