#include "Player.h"
#include "Keyboard.h"

// The player's body is 40x40 by default, so half of it is 20
static constexpr float kHalfSize = 20.0f;
static constexpr float kMoveStep = 20.0f;    // pixels the ship creeps each frame while a move key is held
static constexpr float kBulletSpeed = 12.0f; // pixels the bullet climbs each frame

// The messages the player both asks the engine to send and listens for itself
static const char* const kMoveLeftDown = "PlayerMoveLeft";
static const char* const kMoveLeftUp = "PlayerMoveLeftUp";
static const char* const kMoveRightDown = "PlayerMoveRight";
static const char* const kMoveRightUp = "PlayerMoveRightUp";
static const char* const kFire = "PlayerFire";

Player::Player(CMPUT350::Point2D loc)
    : mLocation(loc)
{
    // One box around the ship, in local space, so the engine places it by GetLocation. Local means the
    // shape stays valid as the player moves; only the location changes.
    mShapes.push_back(CMPUT350::Shape(
        CMPUT350::Rect(CMPUT350::Point2D(-kHalfSize, -kHalfSize), 2.0f * kHalfSize, 2.0f * kHalfSize)));
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    // A and D steer: the down event starts the ship moving, the up event stops it. Space fires on the way down only.
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::A, kMoveLeftDown);
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::A, kMoveLeftUp);
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::D, kMoveRightDown);
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::D, kMoveRightUp);
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Space, kFire);

    // The engine stamps whoever it is calling, so CurrObject is the player itself here
    context->mNotificationManager->Register(context->CurrObject, kMoveLeftDown);
    context->mNotificationManager->Register(context->CurrObject, kMoveLeftUp);
    context->mNotificationManager->Register(context->CurrObject, kMoveRightDown);
    context->mNotificationManager->Register(context->CurrObject, kMoveRightUp);
    context->mNotificationManager->Register(context->CurrObject, kFire);
}

void Player::Update(CMPUT350::GameContext* context)
{
    // Movement happens once per frame while a key is held, so the ship glides instead of stepping at the OS repeat rate.
    // Only x moves: operator+=(float) shifts both axes, so it would send the player diagonally.
    float dx = 0.0f;
    if (mLeftHeld) dx -= kMoveStep;
    if (mRightHeld) dx += kMoveStep;
    if (dx != 0.0f)
    {
        mLocation.x += dx;
        ClampToScreen(context);
    }

    // Fire is spawned here, not in the notification, because only Update is handed a context
    if (mFireRequested)
    {
        mFireRequested = false;
        Fire(context);
    }
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

void Player::ReceiveNotification(const std::string& key)
{
    // Remember which direction keys are down; the movement itself is applied every frame in Update
    if (key == kMoveLeftDown) mLeftHeld = true;
    else if (key == kMoveLeftUp) mLeftHeld = false;
    else if (key == kMoveRightDown) mRightHeld = true;
    else if (key == kMoveRightUp) mRightHeld = false;
    else if (key == kFire) mFireRequested = true;
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

    context->ScreenContext->DrawRect(CMPUT350::Rect(mLocation.x - h, mLocation.y + 0.2f * h, 2.0f * h, 0.8f * h),
                                     CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(CMPUT350::Rect(mLocation.x - 0.3f * h, mLocation.y - h, 0.6f * h, 2.0f * h),
                                     CMPUT350::Colors::grey);
    context->ScreenContext->DrawCircle(mLocation - CMPUT350::Point2D(0.0f, 0.4f * h), 0.3f * h,
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

const std::vector<CMPUT350::Shape>& Player::GetShapes()
{
    return mShapes; // local, so it is built once and handed back unchanged
}

const CMPUT350::Rect& Player::GetBounds() const
{
    // The player moves, so the box is rebuilt from the current centre every time it is asked for
    mBounds = CMPUT350::Rect(mLocation - CMPUT350::Point2D(kHalfSize, kHalfSize), 2 * kHalfSize, 2 * kHalfSize);
    return mBounds;
}


CMPUT350::Point2D Player::GetLocation() const {
    return mLocation;
}

float Player::GetRotation() const {
    return 0;
}
