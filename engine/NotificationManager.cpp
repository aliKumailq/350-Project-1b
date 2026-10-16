#include "NotificationManager.h"

#include <algorithm>

#include "GameObject.h"

namespace CMPUT350 {

void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {
    // A weak pointer costs nothing to hold, so the listener survives even if the manager outlives it
    mListeners[key].push_back(object);
}

void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {
    auto found = mListeners.find(key);
    if (found == mListeners.end()) {
        return;
    }
    auto& listeners = found->second;
    // Compare the locked pointers, since two weak pointers to the same object are not themselves equal
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                                   [&object](const std::weak_ptr<GameObject>& listener) {
                                       return listener.lock() == object.lock();
                                   }),
                    listeners.end());
}

void NotificationManager::Notify(const std::string& message) {
    auto found = mListeners.find(message);
    if (found == mListeners.end()) {
        return;
    }
    auto& listeners = found->second;
    // A message only lands in the log if it actually reached a listener, so a replay carries the events
    // that mattered rather than every shout into the void
    if (mRecording) {
        mRecorded.push_back(message);
    }
    // Deliver right away to everyone still alive, then sweep out the ones that have died so the list
    // does not grow stale over a long game
    for (auto& listener : listeners) {
        if (auto object = listener.lock()) {
            object->ReceiveNotification(message);
        }
    }
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                                   [](const std::weak_ptr<GameObject>& listener) {
                                       return listener.expired();
                                   }),
                    listeners.end());
}

void NotificationManager::StartRecording() {
    // Start from empty, so a replay always matches the run that was captured and nothing stale leaks in
    mRecorded.clear();
    mRecording = true;
}

void NotificationManager::StopRecording() {
    mRecording = false;  // the log is kept, only the capturing stops
}

void NotificationManager::Replay() {
    // Walk a copy, because delivering a message can itself notify and append to the log we are reading
    const std::vector<std::string> recorded = mRecorded;
    for (const auto& message : recorded) {
        Notify(message);
    }
}

}  // namespace CMPUT350
