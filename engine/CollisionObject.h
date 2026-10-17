#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {


class CollisionObject : public GraphicsObject {
public:
    // Told when this object has run into another; the object handed over is the one it hit
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;

    // The engine calls this form, which also carries where the two objects met. An object that does not
    // care about the point leaves this alone and inherits the forward to the one-argument version.
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const Point2D &collisionPoint)
    { CollisionEnter(obj); }

    // The shapes this object collides with, in its own local space. The engine applies GetLocation to
    // place them in the world, so these do not have to match what the object draws.
    virtual const std::vector<Shape> &GetShapes() = 0;

    // A cheap local-space box around the object, used to skip shape tests between objects that are far apart
    virtual const Rect &GetBounds() = 0;
};


}  // namespace CMPUT350

#endif
