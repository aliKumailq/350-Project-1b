#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <memory>

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"
#include "NotificationManager.h"

namespace CMPUT350 {

class GameContext {
public:
    EngineView *mEngineView;
    DrawContext *ScreenContext;             // draws with the current object's transform applied
    DrawContext *GUIContext;                // never transformed, for drawing UI in screen space
    std::weak_ptr<GameObject> CurrObject;   // the object the engine is calling right now
    NotificationManager *mNotificationManager;
    int currentFrame;                       // a global clock, so objects do not each keep their own
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H
