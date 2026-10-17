#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

namespace CMPUT350 {

class GameContext;

// The base of everything the engine runs. The engine owns these through shared_ptr and calls the
// hooks below each frame; an object that does not care about a hook just keeps the empty default.
class GameObject {
public:
    virtual ~GameObject() = default;
    // Called once, right after the object is handed to the engine and before its first Update
    virtual void Initialize(GameContext *context);
    // Called every frame, before collisions are checked
    virtual void Update(GameContext *context);
    // Called every frame, after collisions, for anything that wants to react to them
    virtual void LateUpdate(GameContext *context);
    // A screen-space UI pass. The engine never calls this itself, it is for a game to use
    virtual void RenderUI(GameContext *context);
    // The engine sweeps an object out once this comes back false
    virtual bool IsAlive() const;
    // Mark the object dead, so the engine removes it at the start of the next frame
    virtual void Kill();
    // Delivered when a message this object registered for is shouted through the notification manager
    virtual void ReceiveNotification(const std::string& key);
};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H
