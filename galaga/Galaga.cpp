#include "Galaga.h"

#include <algorithm>

#include "Boss.h"
#include "Goei.h"
#include "Keyboard.h"
#include "Stars.h"
#include "Zako.h"

// The messages Galaga installs and listens for on the title screen
static const char* const kCoin = "GalagaCoin";
static const char* const kStart = "GalagaStart";

static constexpr int kStartLives = 3;
static constexpr int kWavesPerLevel = 5;   // five waves of eight ships make a level
static constexpr int kShipsPerWave = 8;
static constexpr int kStartupFrames = 90;  // how long the "level" card shows
static constexpr int kGameOverFrames = 180; // how long the game-over card shows
static constexpr int kSpawnInterval = 12;  // frames between the ships of a wave streaming in
static constexpr int kAttackInterval = 30; // frames between the game sending a ship down

Galaga::Galaga(int windowWidth, int windowHeight)
    : mState(State::kWaiting), mFrameInState(0), mCoins(0), mScore(0), mLives(kStartLives), mLevel(1),
      mWaveIndex(0), mSpawnIndex(0), mStartRequested(false),
      mWindowWidth(windowWidth), mWindowHeight(windowHeight)
{
}

void Galaga::Initialize(CMPUT350::GameContext* context)
{
    // Galaga installs every key and says which message it sends. The player registers for the ones it
    // cares about in its own Initialize, so the two halves stay agreed through the shared names.
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num5, kCoin);
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num1, kStart);
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::A, kPlayerMoveLeft);
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::A, kPlayerMoveLeftUp);
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::D, kPlayerMoveRight);
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::D, kPlayerMoveRightUp);
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Space, kPlayerFire);

    // Galaga hears the coin and start keys itself; the player's messages go to the player
    context->mNotificationManager->Register(context->CurrObject, kCoin);
    context->mNotificationManager->Register(context->CurrObject, kStart);

    // The starfield is the backdrop for the whole session
    context->mEngineView->AddGameObject(
        std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, float(mWindowWidth), float(mWindowHeight))));

    EnterState(State::kWaiting);
}

void Galaga::Update(CMPUT350::GameContext* context)
{
    mFrameInState++;

    switch (mState)
    {
    case State::kWaiting:
        // The coin and start keys only matter here; they arrive through ReceiveNotification
        if (mStartRequested)
        {
            mStartRequested = false;
            mCoins--;
            StartNewGame(context);
        }
        break;

    case State::kStartup:
        if (mFrameInState >= kStartupFrames) StartLevel();
        break;

    case State::kWaves:
        UpdateWaves(context);
        break;

    case State::kGameplay:
        UpdateGameplay(context);
        break;

    case State::kGameOver:
        if (mFrameInState >= kGameOverFrames) EnterState(State::kWaiting);
        break;
    }
}

void Galaga::LateUpdate(CMPUT350::GameContext* context)
{
}

void Galaga::ReceiveNotification(const std::string& key)
{
    // The coin and start keys only mean anything on the title screen
    if (mState != State::kWaiting) return;

    if (key == kCoin)
    {
        mCoins++;
    }
    else if (key == kStart && mCoins > 0)
    {
        // A key message carries no context, so the game is actually started on the next Update
        mStartRequested = true;
    }
}

bool Galaga::IsAlive() const
{
    return true; // the manager stays for the whole session
}

void Galaga::Kill()
{
}

void Galaga::RenderBackground(CMPUT350::GameContext* context)
{
}

void Galaga::RenderForeground(CMPUT350::GameContext* context)
{
    DrawHud(context);
}

float Galaga::GetRotation() const
{
    return 0.0f;
}

CMPUT350::Point2D Galaga::GetLocation() const
{
    return CMPUT350::Point2D(0.0f, 0.0f); // the HUD is drawn in screen space, so the manager sits at the origin
}

void Galaga::EnterState(State state)
{
    mState = state;
    mFrameInState = 0;
}

void Galaga::StartNewGame(CMPUT350::GameContext* context)
{
    mScore = 0;
    mLives = kStartLives;
    mLevel = 1;
    SpawnPlayer(context);
    EnterState(State::kStartup);
}

void Galaga::SpawnPlayer(CMPUT350::GameContext* context)
{
    // Created once and reused; the engine keeps it alive, so later levels just leave it where it is
    if (mPlayer) return;

    mPlayer = std::make_shared<Player>(
        CMPUT350::Point2D(float(mWindowWidth) / 2.0f, float(mWindowHeight) - 120.0f));
    context->mEngineView->AddGameObject(mPlayer);
}

void Galaga::StartLevel()
{
    mWaveIndex = 0;
    BuildWave(mWaveIndex);
    EnterState(State::kWaves);
}

void Galaga::BuildWave(int wave)
{
    mSpawnPlan.clear();
    const float rowY = 140.0f + float(wave) * 70.0f;

    for (int col = 0; col < kShipsPerWave; col++)
    {
        const CMPUT350::Point2D slot(97.0f + float(col) * 82.0f, rowY);
        // A fixed mix, so all three ships show up: a boss every fourth slot, the two-health ship beside
        // it, and the one-health soldier filling the middle
        const int kind = col % 4;

        // Each ship sweeps in from its own side on a curve that ends at its slot
        const float side = (col % 2 == 0) ? -1.0f : 1.0f;
        const std::vector<CMPUT350::Point2D> path = {
            CMPUT350::Point2D(slot.x + side * 300.0f, -80.0f),
            CMPUT350::Point2D(slot.x + side * 140.0f, 180.0f),
            CMPUT350::Point2D(slot.x - side * 120.0f, 300.0f),
            slot};

        std::shared_ptr<Enemy> enemy;
        if (kind == 0) enemy = std::make_shared<BossEnemy>(slot, path);
        else if (kind == 2) enemy = std::make_shared<ZakoEnemy>(slot, path);
        else enemy = std::make_shared<GoeiEnemy>(slot, path);

        mSpawnPlan.push_back(enemy);
    }

    mSpawnIndex = 0;
}

void Galaga::UpdateWaves(CMPUT350::GameContext* context)
{
    // Release the wave one ship at a time, so they stream in instead of all appearing at once
    if (mSpawnIndex < int(mSpawnPlan.size()))
    {
        if (mFrameInState >= mSpawnIndex * kSpawnInterval)
        {
            auto enemy = mSpawnPlan[mSpawnIndex++];
            context->mEngineView->AddGameObject(enemy);
            mEnemies.push_back(enemy);

            // Once the last ship is out the engine owns them all, so let the plan go
            if (mSpawnIndex == int(mSpawnPlan.size())) mSpawnPlan.clear();
        }
        return;
    }

    // Every ship is out; once they have all settled into their slots the main play begins
    for (auto& weak : mEnemies)
    {
        auto enemy = weak.lock();
        if (enemy && enemy->IsAlive() && !enemy->InFormation()) return;
    }
    EnterState(State::kGameplay);
}

void Galaga::UpdateGameplay(CMPUT350::GameContext* context)
{
    // Clear out the ships that are gone first, so an empty list really means the wave is beaten
    PruneEnemies();

    if (mEnemies.empty())
    {
        mWaveIndex++;
        if (mWaveIndex < kWavesPerLevel)
        {
            BuildWave(mWaveIndex);
            EnterState(State::kWaves);
        }
        else
        {
            // The whole level is cleared, so the next one starts with its own startup card
            mLevel++;
            EnterState(State::kStartup);
        }
        return;
    }

    if (mFrameInState % kAttackInterval == 0) LaunchAttack(context);
}

void Galaga::LaunchAttack(CMPUT350::GameContext* context)
{
    // Gather the settled ships the preferred way: walking the engine's own object list
    std::vector<std::shared_ptr<Enemy>> ready;
    for (auto it = context->mEngineView->begin(); it != context->mEngineView->end(); ++it)
    {
        auto enemy = std::dynamic_pointer_cast<Enemy>(*it);
        if (enemy && enemy->IsAlive() && enemy->InFormation()) ready.push_back(enemy);
    }
    if (ready.empty()) return;

    // Deterministic pick: cycle through the ready ships on the frame clock, so the choice is repeatable
    auto chosen = ready[size_t(mFrameInState / kAttackInterval) % ready.size()];
    const CMPUT350::Point2D target = mPlayer ? mPlayer->GetLocation()
                                             : CMPUT350::Point2D(float(mWindowWidth) / 2.0f, float(mWindowHeight));
    chosen->Attack(context, target);
}

void Galaga::PruneEnemies()
{
    // A ship that is gone, or dead and about to be removed, no longer counts as part of the wave
    mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(),
                                  [](const std::weak_ptr<Enemy>& weak) {
                                      auto enemy = weak.lock();
                                      return !enemy || !enemy->IsAlive();
                                  }),
                   mEnemies.end());
}

void Galaga::DrawHud(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* gui = context->GUIContext;
    const float w = float(mWindowWidth);
    const float h = float(mWindowHeight);

    // Score on the left, level in the middle, lives on the right, all along the top
    gui->DrawText("SCORE " + std::to_string(mScore), 24, CMPUT350::Point2D(20.0f, 20.0f), CMPUT350::Colors::white);
    gui->DrawCenteredText("LEVEL " + std::to_string(mLevel), 24, CMPUT350::Point2D(w / 2.0f, 20.0f),
                          CMPUT350::Colors::white);
    gui->DrawText("LIVES " + std::to_string(mLives), 24, CMPUT350::Point2D(w - 170.0f, 20.0f),
                  CMPUT350::Colors::white);

    if (mState == State::kWaiting)
    {
        gui->DrawCenteredText("INSERT COIN - PRESS 5", 30, CMPUT350::Point2D(w / 2.0f, h / 2.0f - 40.0f),
                              CMPUT350::Colors::yellow);
        gui->DrawCenteredText("PRESS 1 TO START", 30, CMPUT350::Point2D(w / 2.0f, h / 2.0f + 20.0f),
                              CMPUT350::Colors::yellow);
        gui->DrawCenteredText("COINS  " + std::to_string(mCoins), 24, CMPUT350::Point2D(w / 2.0f, h / 2.0f + 80.0f),
                              CMPUT350::Colors::grey);
    }
    else if (mState == State::kStartup)
    {
        gui->DrawCenteredText("LEVEL " + std::to_string(mLevel), 40, CMPUT350::Point2D(w / 2.0f, h / 2.0f),
                              CMPUT350::Colors::cyan);
    }
    else if (mState == State::kGameOver)
    {
        gui->DrawCenteredText("GAME OVER", 46, CMPUT350::Point2D(w / 2.0f, h / 2.0f), CMPUT350::Colors::red);
    }
}
