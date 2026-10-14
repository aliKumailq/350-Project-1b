#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "CollisionObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

// True when the two objects' shapes overlap. When they do, collisionPoint is set to a point where the
// two met. Objects report their shapes in local space, so this translates them by GetLocation first.
bool collision(CollisionObject &obj1, CollisionObject &obj2, Point2D &collisionPoint);

}  // namespace CMPUT350

#endif
