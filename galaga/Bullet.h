#ifndef BULLET_H
#define BULLET_H

#include "CollisionObject.h"
#include "GameContext.h"
#include "MathUtil.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
    // parent is whoever fired the bullet; the bullet watches it weakly so it can die with its owner
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player,
           std::weak_ptr<CMPUT350::GameObject> parent);
    bool IsPlayerBullet();

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
	float GetRotation() const override;
    CMPUT350::Point2D GetLocation() const override;


    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                        const CMPUT350::Point2D& collisionPoint) override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    const CMPUT350::Rect& GetBounds()  override;

private:
    // mPreviousLocation is kept so the bounds can cover the whole step, not just where the bullet ended up
    CMPUT350::Point2D mLocation, mPreviousLocation, mHeading;
    bool mPlayer, mAlive;
    std::weak_ptr<CMPUT350::GameObject> mParent; // whoever fired it; an expired parent takes the bullet with it
    CMPUT350::Point2D mCollisionPoint;           // where the bullet struck, for the death-mark it leaves
    bool mHit;                                   // true once it has a strike point to draw instead of its body
    mutable CMPUT350::Rect mBounds;        // GetBounds returns a reference, so something has to own the rectangle
    std::vector<CMPUT350::Shape> mShapes;  // rebuilt each call: the swept segment moves with the bullet
};
#endif // BULLET_H
