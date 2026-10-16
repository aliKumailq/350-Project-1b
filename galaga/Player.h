#ifndef PLAYER_H
#define PLAYER_H

#include "Bullet.h"
#include "CollisionObject.h"

// The messages the player listens for. The Galaga class installs the keys that send them, so both sides
// agree on these names through this header instead of repeating the strings.
inline constexpr const char* kPlayerMoveLeft = "PlayerMoveLeft";
inline constexpr const char* kPlayerMoveLeftUp = "PlayerMoveLeftUp";
inline constexpr const char* kPlayerMoveRight = "PlayerMoveRight";
inline constexpr const char* kPlayerMoveRightUp = "PlayerMoveRightUp";
inline constexpr const char* kPlayerFire = "PlayerFire";
// The message the player sends when it is destroyed, which the Galaga class listens for
inline constexpr const char* kPlayerDestroyed = "PlayerDestroyed";

class Player : public CMPUT350::CollisionObject
{
public:
    Player(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;
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

    // A destroyed player is put back into play by the Galaga class once the ships have gone home
    void Revive();
    bool IsDestroyed() const;

private:
    void Fire(CMPUT350::GameContext* context);
    void ClampToScreen(CMPUT350::GameContext* context);

    CMPUT350::Point2D mLocation;         // the player's centre
    CMPUT350::Point2D mSpawnLocation;    // where the ship sits when it starts or is re-spawned
    std::weak_ptr<Bullet> mBullets[2];   // the player watches the two shots the engine owns; an empty slot is a free one
    std::vector<CMPUT350::Shape> mShapes; // the ship's hitbox, in local space, so it never changes as the player moves
    mutable CMPUT350::Rect mBounds;      // GetBounds returns a reference, so something has to own the rectangle

    // Held-key state, so a held key keeps moving the ship every frame instead of only once per OS key repeat
    bool mLeftHeld = false;
    bool mRightHeld = false;
    bool mFireRequested = false;         // a fire message carries no context, so the shot is spawned on the next Update

    bool mDestroyed = false;             // out of play until the Galaga class re-spawns it
    bool mDeathNotified = false;         // so the death message is sent exactly once
};

#endif
