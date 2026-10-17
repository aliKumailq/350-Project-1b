#ifndef GALAGA_H
#define GALAGA_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Enemy.h"
#include "GraphicsObject.h"
#include "Player.h"

inline constexpr const char* kCoin = "GalagaCoin";
inline constexpr const char* kStart = "GalagaStart";
inline constexpr const char* kKeySetup = "GalagaKeySetup";

// Galaga owns the whole game. It creates and removes every object, watches the keyboard, draws the score
// and lives, and moves through a small state machine: waiting for a coin, a short startup screen, the
// waves of ships flying in, the main play, and the game-over screen.
class Galaga : public CMPUT350::GraphicsObject
{
public:
    Galaga(int windowWidth, int windowHeight);

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

private:
    struct Binding {
        int scancode;
        const char* label;
        const char* downMsg;
        const char* upMsg;
    };
    std::vector<Binding> mBindings;

    enum class State { kWaiting, kKeySetup, kStartup, kWaves, kGameplay, kGameOver };

    void InstallBindings(CMPUT350::GameContext* context);
    void BeginKeyCapture(CMPUT350::GameContext* context);
    std::string KeyText(int scancode) const;
    std::string KeyTextFor(const char* downMsg) const;
    // Move to a state and restart its frame timer
    void EnterState(State state);
    // Reset the score, lives and level, put the player on screen, and show the startup screen
    void StartNewGame(CMPUT350::GameContext* context);
    // Put the player on screen the first time it is needed
    void SpawnPlayer(CMPUT350::GameContext* context);
    // Begin a level: its first wave
    void StartLevel();
    // Build one wave of eight ships into the spawn queue
    void BuildWave(int wave);
    // Feed the queued ships out over time, then move on once they have all settled
    void UpdateWaves(CMPUT350::GameContext* context);
    // Send ships down to attack, and watch for the wave being cleared
    void UpdateGameplay(CMPUT350::GameContext* context);
    // Ask one settled ship to dive at the player, choosing it from the engine's object list
    void LaunchAttack(CMPUT350::GameContext* context);
    // Score the ships that have died and drop them, so an empty list means the wave is cleared
    void SweepEnemies();
    // True when no living ship is still away from its slot
    bool AllSettled() const;
    // A life is spent; the re-spawn waits until the ships are home
    void OnPlayerDestroyed();
    // Clear the field and show the game-over screen
    void EndGame();
    // Draw the score, lives, level and any state message
    void DrawHud(CMPUT350::GameContext* context);

    State mState;
    int mFrameInState;
    int mCoins;
    int mScore;
    int mLives;
    int mLevel;
    int mWaveIndex;                              // which of the level's five waves is out
    int mSpawnIndex;                             // how many of the current wave's ships have been released
    bool mStartRequested;                        // the start key was pressed; handled on the next Update
    bool mRespawnPending;                        // the player is down; put it back once the ships are home
    bool mKeySetupRequested;                     // the setup key was pressed; handled on the next Update
    bool mKeySetupExitRequested;                 // the back key was pressed on the setup screen
    int mRebindIndex;                            // which row of mBindings the setup screen is editing

    std::unordered_map<std::string, int> mCaptureNames;  // "GalagaKey_<n>" -> n, built once in Initialize
    int mPendingScancode;                                // the key captured this frame; -1 before any

    std::vector<std::shared_ptr<Enemy>> mSpawnPlan;   // the wave waiting to be released, one ship at a time
    std::vector<std::weak_ptr<Enemy>> mEnemies;  // the ships out on screen, watched weakly
    std::shared_ptr<Player> mPlayer;

    int mWindowWidth;
    int mWindowHeight;
};

#endif
