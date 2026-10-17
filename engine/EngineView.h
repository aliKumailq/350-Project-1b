#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <string>
#include <vector>

namespace CMPUT350 {

class GameObject;

class EngineView {
    // How the engine stores what it holds; the iterators below hand this out without exposing the vector
    using tGameObject = std::vector<std::shared_ptr<GameObject>>;

public:
    // Hand the engine an object to own and run; it starts being updated on the next frame
    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;

    // Register a message to fire when the given key scancode is pressed down or released
    virtual void InstallKeyDownNotification(int scancode, const std::string &notification) = 0;
    virtual void InstallKeyUpNotification(int scancode, const std::string &notification) = 0;

    // Read-only iteration over the objects the engine is currently holding
    using const_iterator = tGameObject::const_iterator;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};

}  // namespace CMPUT350

#endif
