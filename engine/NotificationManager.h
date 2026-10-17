#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CMPUT350 {

class GameObject;

// Lets objects talk to each other without holding references to one another: a listener
// registers for a named message, and anyone can shout that message into the manager
class NotificationManager {
public:
    // A listener asks to hear this message. The pointer is weak, so registering never keeps it alive
    void Register(std::weak_ptr<GameObject> object, const std::string& key);
    // Stop delivering this message to this object
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);
    // Shout a message: every live listener for it hears it right away, and the dead ones are swept out
    void Notify(const std::string& message);

    // Recording keeps a log of every message that reaches a listener, so a run can be captured and
    // played back later instead of being re-driven by hand
    // Begin logging delivered messages, from empty
    void StartRecording();
    // Stop logging, but keep what was logged
    void StopRecording();
    // Send the logged messages again, in the order they were delivered
    void Replay();

private:
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
    bool mRecording = false;
    std::vector<std::string> mRecorded;  // the messages that were delivered, in the order they arrived
};

}  // namespace CMPUT350

#endif  // NOTIFICATION_MANAGER_H
