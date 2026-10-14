#ifndef PLAYER_H
#define PLAYER_H

#include "Bullet.h"
#include "CollisionObject.h"

class Player : public CMPUT350::CollisionObject
{
public:
    Player(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    float GetRotation() const override;
    CMPUT350::Point2D GetLocation() const override;


    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds()  override;
    const std::vector<CMPUT350::Shape> &GetShapes()  override;

private:
    void Fire(CMPUT350::GameContext* context);
    void ClampToScreen(CMPUT350::GameContext* context);

    CMPUT350::Point2D mLocation;         // the player's centre
    std::weak_ptr<Bullet> mBullets[2];   // the player watches the two shots the engine owns; an empty slot is a free one
    CMPUT350::Rect mBounds;              // GetBounds returns a reference, so something has to own the rectangle
    std::vector<CMPUT350::Shape> mShapes;
};

#endif
