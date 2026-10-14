#include "Player.h"
#include "MathUtil.h"

// The player's body is 40x40 by default, so half of it is 20
static constexpr float kHalfSize = 20.0f;
static constexpr float kMoveStep = 20.0f;    // pixels moved by a single A or D press
static constexpr float kBulletSpeed = 12.0f; // pixels the bullet climbs each frame

Player::Player(CMPUT350::Point2D loc)
    : mLocation(loc), mShapes{CMPUT350::Rect(mLocation - CMPUT350::Point2D(kHalfSize, kHalfSize), 2 * kHalfSize, 2 * kHalfSize)}
{
}

void Player::Initialize(CMPUT350::GameContext* context)
{
}

void Player::Update(CMPUT350::GameContext* context)
{
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    // Lower case is an ordinary press, upper case is the same key pressed with shift held
    if (key == 'a' || key == 'A')
    {
        // Only x moves: operator+=(float) shifts both axes, so it would send the player diagonally
        mLocation.x -= kMoveStep;
        ClampToScreen(context);
        return (true);
    }

    if (key == 'd' || key == 'D')
    {
        mLocation.x += kMoveStep;
        ClampToScreen(context);
        return (true);
    }

    if (key == ' ')
    {
        Fire(context);
        return (true); // space is the player's key even when both shots are still in flight
    }

    return (false);
}

void Player::Fire(CMPUT350::GameContext* context)
{
    // An empty weak_ptr has always expired, so the bullet being gone vs it never fired are the same
    std::weak_ptr<Bullet>* slot = nullptr;
    for (auto& bullet : mBullets)
    {
        if (bullet.expired())
        {
            slot = &bullet;
            break;
        }
    }

    if (slot == nullptr) return; // both slots are still occupied, so the player can't fire

    // The nose sits on the player's top edge, so the two boxes touch without ever overlapping on the spawn frame
    auto fired = std::make_shared<Bullet>(mLocation - CMPUT350::Point2D(0.0f, kHalfSize),
                                          CMPUT350::Point2D(0.0f, -kBulletSpeed), true);

    // The engine owns the bullet from here on, so the player only watches it through the weak_ptr
    *slot = fired;
    context->mEngineView->AddGameObject(fired);
}

void Player::ClampToScreen(CMPUT350::GameContext* context)
{
    // The whole body has to stay on screen, so the centre stops half a body short of either edge
    float furthestRight = context->ScreenContext->GetWindowWidth() - kHalfSize;

    if (mLocation.x < kHalfSize) mLocation.x = kHalfSize;
    else if (mLocation.x > furthestRight) mLocation.x = furthestRight;
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    // Each shape is a fraction of the hitbox half-size, so the ship can never reach outside the box it collides with. Later calls paint over earlier ones, so this runs back to front: wings, then body, then cockpit.
    const float h = kHalfSize;

    context->ScreenContext->DrawRect(CMPUT350::Rect(0 - h, 0 + 0.2f * h, 2.0f * h, 0.8f * h),
                                     CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(CMPUT350::Rect(0 - 0.3f * h, 0 - h, 0.6f * h, 2.0f * h),
                                     CMPUT350::Colors::grey);
    context->ScreenContext->DrawCircle(CMPUT350::Point2D(0,0) - CMPUT350::Point2D(0.0f, 0.4f * h), 0.3f * h,
                                       CMPUT350::Colors::cyan);
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // Bullets do not destroy the player, and the player has nothing else to collide with currently
}

void Player::Kill()
{
    // The player cannot be destroyed, so there is no dead state to enter
}

bool Player::IsAlive() const
{
    return (true); // the player is permanent, so it never leaves the engine
}

const CMPUT350::Rect& Player::GetBounds()
{
    // The player moves, so the box is rebuilt from the current centre every time it is asked for
    mShapes[0] = CMPUT350::Rect(mLocation - CMPUT350::Point2D(kHalfSize, kHalfSize), 2 * kHalfSize, 2 * kHalfSize);
    return mShapes[0].shape.rect;
}


CMPUT350::Point2D Player::GetLocation() const {
    return CMPUT350::Point2D(0,0);
}

float Player::GetRotation() const {
    return 0;
}


const std::vector<CMPUT350::Shape> &Player::GetShapes()  {
    return mShapes;
}
