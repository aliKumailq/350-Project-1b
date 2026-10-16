#include "Player.h"

#include "Enemy.h"

// The player's body is 40x40 by default, so half of it is 20
static constexpr float kHalfSize = 20.0f;
static constexpr float kMoveStep = 20.0f;    // pixels the ship creeps each frame while a move key is held
static constexpr float kBulletSpeed = 12.0f; // pixels the bullet climbs each frame

Player::Player(CMPUT350::Point2D loc)
    : mLocation(loc), mSpawnLocation(loc)
{
    // One box around the ship, in local space, so the engine places it by GetLocation. Local means the
    // shape stays valid as the player moves; only the location changes.
    mShapes.push_back(CMPUT350::Shape(
        CMPUT350::Rect(CMPUT350::Point2D(-kHalfSize, -kHalfSize), 2.0f * kHalfSize, 2.0f * kHalfSize)));
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    // The Galaga class installs the keys that send these messages; the player only says which ones it
    // wants to hear. The engine stamps whoever it is calling, so CurrObject is the player itself here.
    context->mNotificationManager->Register(context->CurrObject, kPlayerMoveLeft);
    context->mNotificationManager->Register(context->CurrObject, kPlayerMoveLeftUp);
    context->mNotificationManager->Register(context->CurrObject, kPlayerMoveRight);
    context->mNotificationManager->Register(context->CurrObject, kPlayerMoveRightUp);
    context->mNotificationManager->Register(context->CurrObject, kPlayerFire);
}

void Player::Update(CMPUT350::GameContext* context)
{
    // A destroyed ship is out of play: it neither moves nor fires until the Galaga class re-spawns it
    if (mDestroyed) return;

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
    // A ship destroyed this frame tells the game once. CollisionEnter has no context, so the message waits
    // for here, which still runs the same frame.
    if (mDestroyed && !mDeathNotified)
    {
        mDeathNotified = true;
        context->mNotificationManager->Notify(kPlayerDestroyed);
    }
}

void Player::ReceiveNotification(const std::string& key)
{
    // Remember which direction keys are down; the movement itself is applied every frame in Update
    if (key == kPlayerMoveLeft) mLeftHeld = true;
    else if (key == kPlayerMoveLeftUp) mLeftHeld = false;
    else if (key == kPlayerMoveRight) mRightHeld = true;
    else if (key == kPlayerMoveRightUp) mRightHeld = false;
    else if (key == kPlayerFire) mFireRequested = true;
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

    // The nose sits on the player's top edge, so the two boxes touch without ever overlapping on the spawn frame.
    // The bullet is told the player is its parent, so it dies if the player ever does.
    auto fired = std::make_shared<Bullet>(mLocation - CMPUT350::Point2D(0.0f, kHalfSize),
                                          CMPUT350::Point2D(0.0f, -kBulletSpeed), true, context->CurrObject);

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
    // A destroyed ship is not drawn; it is off the board until the Galaga class re-spawns it
    if (mDestroyed) return;

    // Each shape is a fraction of the hitbox half-size, so the ship can never reach outside the box it collides with. They are drawn around the origin and the engine adds the ship's location, so the picture travels with it. Later calls paint over earlier ones, so this runs back to front: wings, then body, then cockpit.
    const float h = kHalfSize;

    context->ScreenContext->DrawRect(CMPUT350::Rect(-h, 0.2f * h, 2.0f * h, 0.8f * h),
                                     CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(CMPUT350::Rect(-0.3f * h, -h, 0.6f * h, 2.0f * h),
                                     CMPUT350::Colors::grey);
    context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.0f, -0.4f * h), 0.3f * h,
                                       CMPUT350::Colors::cyan);
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // An enemy shot or an enemy ship destroys the player; the player's own bullets are ignored
    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr)
    {
        if (!bullet->IsPlayerBullet()) mDestroyed = true;
        return;
    }

    if (std::dynamic_pointer_cast<Enemy>(obj) != nullptr) mDestroyed = true;
}

void Player::Kill()
{
    // The player is re-spawned rather than removed, so it never leaves the engine through here
}

bool Player::IsAlive() const
{
    return true; // the player stays in the engine so the Galaga class can re-spawn it
}

void Player::Revive()
{
    // Put the ship back at its start, clear the keys it may still be holding, and let it play again
    mDestroyed = false;
    mDeathNotified = false;
    mLeftHeld = false;
    mRightHeld = false;
    mFireRequested = false;
    mLocation = mSpawnLocation;
}

bool Player::IsDestroyed() const
{
    return mDestroyed;
}

const std::vector<CMPUT350::Shape>& Player::GetShapes()
{
    // A destroyed ship is out of play, so it collides with nothing while it waits to be re-spawned
    static const std::vector<CMPUT350::Shape> none;
    if (mDestroyed) return none;
    return mShapes; // local, so it is built once and handed back unchanged
}

const CMPUT350::Rect& Player::GetBounds() 
{
    if (mDestroyed)
    {
        // A zero box keeps the broad phase from ever pairing it while it is out of play
        mBounds = CMPUT350::Rect(CMPUT350::Point2D(0.0f, 0.0f), 0.0f, 0.0f);
        return mBounds;
    }

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
