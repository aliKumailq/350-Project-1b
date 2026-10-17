#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

class GameContext;
class Point2D;

// A GameObject that can draw. Before each pass the engine sets the draw context to this object's
// location and rotation, so everything drawn here is in the object's own local space.
class GraphicsObject : public GameObject {
public:
    // Drawn first, under everything else, once per frame
    virtual void RenderBackground(GameContext *context);
    // Drawn after every background, over the top
    virtual void RenderForeground(GameContext *context);
    // Center of object with respect to rotation
	virtual Point2D GetLocation() const = 0;
	// Return the rotation of the object in radians
	virtual float GetRotation() const = 0;


private:
};

}  // namespace CMPUT350

#endif
