#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"

class Enemy : public CMPUT350::CollisionObject
{
public:
    Enemy(CMPUT350::Point2D loc);

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
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    const CMPUT350::Rect& GetBounds() const override;

private:
    bool mAlive;
    CMPUT350::Point2D mLocation;          // the enemy's centre, which the engine uses to place its shapes
    std::vector<CMPUT350::Shape> mShapes; // local box, fixed, so it stays valid once enemies start moving
    mutable CMPUT350::Rect mBounds;       // world-space box, rebuilt from mLocation when asked
};


#endif
