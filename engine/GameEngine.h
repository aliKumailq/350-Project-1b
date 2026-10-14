
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

namespace CMPUT350 {
class GameEngine;
}

#include "EngineView.h"
#include "GameObject.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

namespace CMPUT350 {

class DrawContext;
class GameContext;

class GameEngine : public EngineView {
public:
    GameEngine(unsigned int width, unsigned int height, const std::string& name);
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

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

        bool mCloseGame;

        inline void PollWindow(GameContext* context);
        inline void ProcessCollisions();
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H
