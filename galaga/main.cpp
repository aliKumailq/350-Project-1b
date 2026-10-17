#include "DrawContext.h"
#include "Galaga.h"
#include "GameEngine.h"
#include "MathUtil.h"
#include "Player.h"

int main() {
 
        CMPUT350::GameEngine engine(768, 1024, "Galaga");
        auto game = std::make_shared<Galaga>(768, 1024);
        engine.AddGameObject(game);
        engine.Run();

    return 0;
}