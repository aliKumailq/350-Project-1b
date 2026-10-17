
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

namespace CMPUT350 {
class GameEngine;
}

#include "EngineView.h"
#include "GameObject.h"
#include "MathUtil.h"
#include "NotificationManager.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

namespace CMPUT350 {

class DrawContext;
class GameContext;

class GameEngine : public EngineView {
public:
    // Opens a window of the given size and loads the built-in font
    GameEngine(unsigned int width, unsigned int height, const std::string& name);
    // Closes the window if Run() left it open
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

    // Queue an object for the engine to own. It is queued rather than added at once, so it is safe
    // to spawn mid-frame; it gets initialized at the top of the next frame.
    void AddGameObject(std::shared_ptr<GameObject> gameObject) override;

    // Keys the game cares about; the engine fires these strings when the key goes down or comes up
    void InstallKeyDownNotification(int scancode, const std::string &notification) override;
    void InstallKeyUpNotification(int scancode, const std::string &notification) override;

    // Read-only access to the objects the engine is holding, so a game can scan the scene
    EngineView::const_iterator cbegin() const override;
    EngineView::const_iterator cend() const override;
    EngineView::const_iterator begin() const override;
    EngineView::const_iterator end() const override;

    void Run();

private:
    	std::shared_ptr<sf::RenderWindow> mWindow;
    	std::shared_ptr<sf::Font> mFont;
        std::vector<std::shared_ptr<GameObject>> mGameObjects;
        std::vector<std::shared_ptr<GameObject>> mLateGameObjects;

        // Scancode -> notification string, filled by InstallKey*Notification and read on each key event
        std::unordered_map<int, std::string> mKeyDownNotifications;
        std::unordered_map<int, std::string> mKeyUpNotifications;

        NotificationManager mNotificationManager;

        bool mCloseGame;

        // Drain the window's events and turn key presses and releases into their notifications
        inline void PollWindow();
        // Check every pair of colliders once and tell both sides when they touch
        inline void ProcessCollisions();
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H
