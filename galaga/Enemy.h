#ifndef ENEMY_H
#define ENEMY_H

#include <vector>

#include "Bezier.h"
#include "CollisionObject.h"
#include "GameContext.h"

// The message an enemy sends when it is destroyed. The Galaga class listens for it to score the kill.
inline constexpr const char* kEnemyDestroyed = "EnemyDestroyed";

// Enemy is the base class for every hostile ship. Everything the types share lives here: flying in along
// a bezier entry path, hovering in the formation, diving along an attack path and coming back, losing
// health, and telling the game when it dies. A subclass only answers Attack (which curve it dives on)
// and draws itself, so the three enemy types stay short.
class Enemy : public CMPUT350::CollisionObject
{
public:
    // loc is the slot the enemy settles into; entryPath is the bezier it flies in on, ending at that
    // slot; health is how many player hits it survives; points is what a kill is worth.
    Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& entryPath, int health, int points);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    float GetRotation() const override;
    CMPUT350::Point2D GetLocation() const override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    const CMPUT350::Rect& GetBounds() const override;

    // Ask the enemy to break formation and dive at the player. True means the run started; false means it
    // is busy (still flying in, already diving, returning, or dead). Each type answers with its own curve.
    virtual bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) = 0;

    // True only while the enemy is hovering in its slot, so the game sends settled ships to attack.
    bool InFormation() const;

    // What killing this enemy is worth. A ship away from its slot is worth double, as the spec asks.
    int Points() const;

protected:
    // A subclass's Attack builds its dive curve and hands it over; this then owns the flying out and back.
    bool BeginAttack(const std::vector<CMPUT350::Point2D>& attackPath);

    // Draws the standard body in the given colour, dimmed once the ship has been hit. Types pass different
    // colours so they stay visually distinct, and may draw extra shapes around this.
    void DrawBody(CMPUT350::GameContext* context, CMPUT350::RGBColor base) const;

    // The colour a ship shows at its current health: its full colour, then darker once hit, so a two-health
    // ship reads as damaged where a one-health ship would already be gone.
    CMPUT350::RGBColor HealthColor(CMPUT350::RGBColor base) const;

    int mHealth;
    int mMaxHealth;
    int mPoints;
    bool mAlive;

private:
    enum class Phase { kEntering, kFormation, kAttacking, kReturning };

    // Sets the location and rotation from the active curve at the given t.
    void PlaceOnCurve(float t);
    // Bobs the ship around its slot on the shared frame clock, so the whole formation moves together.
    void HoverInFormation(CMPUT350::GameContext* context);
    // Drops one bullet straight down, partway through a dive.
    void Fire(CMPUT350::GameContext* context);

    CMPUT350::Point2D mLocation;
    float mRotation;
    CMPUT350::Point2D mFormation;              // the slot the ship belongs to
    std::vector<CMPUT350::Point2D> mPath;      // the control points of the curve being followed
    Phase mPhase;
    float mT;                                  // progress along mPath, in curve units (0 .. mMaxT)
    float mMaxT;                               // the far end of mPath
    bool mFiredThisDive;
    bool mNotified;                            // so the death message is sent exactly once

    std::vector<CMPUT350::Shape> mShapes;      // local box, fixed, so it stays valid while moving
    mutable CMPUT350::Rect mBounds;            // world box, rebuilt from mLocation when asked
};

#endif
