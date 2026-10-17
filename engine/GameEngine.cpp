#include "GameEngine.h"
#include "GameContext.h"
#include "DrawContext.h"
#include "GraphicsObject.h"
#include "CollisionObject.h"
#include "CollisionDetection.h"
#include "Helpers.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name)
: mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width,height}), name)),
 mFont(std::make_shared<sf::Font>()), mCloseGame(false) {
    // Sample font loading code
    	if (!mFont->openFromMemory(&_font, _font_len))
    	{
    	    fprintf(stderr, "WARNING: Font did not load.\n");
    	}
}

inline void GameEngine::PollWindow() {
	while (const std::optional event = mWindow->pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			mCloseGame = true; // latched, since a later event must not clear it
		}
		else if (event->is<sf::Event::Resized>())
		{
		}
		else if (const auto* keyDown = event->getIf<sf::Event::KeyPressed>())
		{
			// The scancode is the physical key, so it does not move when the layout changes; that is what the Key:: constants describe
			int scancode = static_cast<int>(keyDown->scancode);
			// Only fire if some object asked to hear this key go down
			if (mKeyDownNotifications.find(scancode) != mKeyDownNotifications.end()) {
				mNotificationManager.Notify(mKeyDownNotifications[scancode]);
			}
		}
		else if (const auto* keyUp = event->getIf<sf::Event::KeyReleased>())
		{
			int scancode = static_cast<int>(keyUp->scancode);
			if (mKeyUpNotifications.find(scancode) != mKeyUpNotifications.end()) {
				mNotificationManager.Notify(mKeyUpNotifications[scancode]);
			}
		}
	}
}


// The broad phase (does the pair's bounding boxes even touch) now lives inside collision(), so this
// just walks the candidate pairs and asks the real shape test.
inline void GameEngine::ProcessCollisions() {
    // First figure out who can collide. Then check which pairs are actually touching.

    // build a list containing only objects that are capable of collisions, so we have just the objects capable of collisions before we start comparing anything
    std::vector<std::shared_ptr<CollisionObject>> colliders;
    colliders.reserve(mGameObjects.size());
    for (auto& obj : mGameObjects) {
        // Try to treat this GameObject as a CollisionObject. If it really is one, col_ptr is valid and we keep it, if it isn't, the cast gives nullptr and we ignore it.
        if (std::shared_ptr<CMPUT350::CollisionObject> col_ptr = std::dynamic_pointer_cast<CollisionObject>(obj)) {
            colliders.push_back(col_ptr);
        }
    }

    // Compare every collider against every OTHER collider exactly once.
    //
    // For example with A, B, C:
    //
    // i = A: check A-B, A-C
    // i = B: check B-C
    // i = C: nothing left
    //
    // We never check A-A and we never later repeat B-A.
    for (size_t i = 0; i < colliders.size(); i++) {
        // Starting at i + 1 means:
        //   - don't collide an object with itself
        //   - don't check the same pair twice
        for (size_t j = i + 1; j < colliders.size(); j++) {
            // Being CollisionObjects only means they CAN collide. Overlaps answers if their bounding boxes actually touching or overlapping
            CMPUT350::Point2D pnt;
            bool lineInvolved = false;
            // collision(*colliders[i], *colliders[j], pnt)
            if (collision(*colliders[i], *colliders[j], pnt, lineInvolved)) {
                // If they are overlapping, tell BOTH objects about the same collision; The pair itself is checked only once, but each participant gets its own notification.
                if(!lineInvolved) {
                    colliders[i]->CollisionEnter(colliders[j]);
                    colliders[j]->CollisionEnter(colliders[i]);
                } else {
                
                auto pnt_i = Helpers::TransformPoint(pnt,colliders[i]->GetLocation(), colliders[i]->GetRotation(), true);
                auto pnt_j = Helpers::TransformPoint(pnt,colliders[j]->GetLocation(), colliders[j]->GetRotation(),true);
                // Have to apply the reverse transformation relative to each collision object so they can act accordingly.
                
                colliders[i]->CollisionEnter(colliders[j], pnt_i);
                colliders[j]->CollisionEnter(colliders[i], pnt_j);

                }
            }
        }
    }
}

GameEngine::~GameEngine() {
    // Run() can return while the window is still open, so close it here as a failsafe
    mWindow->close();
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mLateGameObjects.push_back(gameObject);
}

void GameEngine::InstallKeyDownNotification(int scancode, const std::string &notification) {
    mKeyDownNotifications[scancode] = notification;
}

void GameEngine::InstallKeyUpNotification(int scancode, const std::string &notification) {
    mKeyUpNotifications[scancode] = notification;
}

// The engine owns the object list, so it is the one that can hand out iterators over it
EngineView::const_iterator GameEngine::cbegin() const { return mGameObjects.cbegin(); }
EngineView::const_iterator GameEngine::cend() const { return mGameObjects.cend(); }
EngineView::const_iterator GameEngine::begin() const { return mGameObjects.begin(); }
EngineView::const_iterator GameEngine::end() const { return mGameObjects.end(); }

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */
void GameEngine::Run() {

    GameContext ctx;
    ctx.mEngineView = this;
    DrawContext drawCtx = DrawContext(mWindow, mFont);
    ctx.ScreenContext = &drawCtx;
    // A second context for UI: it is never handed an object's transform, so it draws in screen space
    DrawContext guiCtx = DrawContext(mWindow, mFont);
    ctx.GUIContext = &guiCtx;
    ctx.mNotificationManager = &mNotificationManager;
    ctx.currentFrame = 0;

    mWindow->setFramerateLimit(30); // Needed since physics are tied to the engine.
    // Not sure if this is supposed to be hardcoded or if the user sets it yet.


    while (mWindow->isOpen())  // window is open
    {



        // 0. Remove any objects that are now dead
        auto remove_inds = std::remove_if(mGameObjects.begin(),mGameObjects.end(), [](const std::shared_ptr<GameObject>& obj) -> bool {
            return !(obj->IsAlive());
        });

        mGameObjects.erase(remove_inds, mGameObjects.end()); // if only we were on c++20

        // 1. Activate and initialize any objects added during the last frame
        size_t firstNew = mGameObjects.size();
        // Make enough room before moving the waiting objects in.
        mGameObjects.reserve(firstNew + mLateGameObjects.size());
        // Move all objects from the queue into the real game-object list.
        mGameObjects.insert(mGameObjects.end(), std::make_move_iterator(mLateGameObjects.begin()), std::make_move_iterator(mLateGameObjects.end()));
        // They're now in mGameObjects, so the queue is empty.
        mLateGameObjects.clear();

        // Initialize ONLY the objects we just added. firstNew marks where the old objects stop and the new objects begin.
        for (size_t i = firstNew; i < mGameObjects.size(); i++) { ctx.CurrObject = mGameObjects[i]; mGameObjects[i]->Initialize(&ctx); }

        // 2. Process events
        PollWindow();
        if (mCloseGame) return;

        // 3. Update game objects
        // Stamp the object being called so anything it creates can hold a weak reference back to it
        for (auto& obj : mGameObjects) { ctx.CurrObject = obj; obj->Update(&ctx); }

        // 4. Process collision events
        ProcessCollisions();

        // 5. Late updates
        for (auto& obj : mGameObjects) obj->LateUpdate(&ctx);

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (auto& obj : mGameObjects) {
            if (std::shared_ptr<CMPUT350::GraphicsObject> ptr = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
                drawCtx.SetContextOffset(ptr->GetLocation());
                drawCtx.SetContextRotation(ptr->GetRotation());
                ptr->RenderBackground(&ctx); }
                // this is bad for performance but the spec said it's fine.
        }

        // 7. Render foreground
        for (auto& obj : mGameObjects) {
            if (std::shared_ptr<CMPUT350::GraphicsObject> ptr = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
                drawCtx.SetContextOffset(ptr->GetLocation());
                drawCtx.SetContextRotation(ptr->GetRotation());
                ptr->RenderForeground(&ctx); }
                // this is bad for performance but the spec said it's fine.
        }


        // Actually render to window
        mWindow->display();

        // A global clock, so no object has to track frames on its own
        ctx.currentFrame++;
    }
}

}  // namespace CMPUT350
