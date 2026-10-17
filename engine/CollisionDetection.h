#include "CollisionObject.h"
#include "MathUtil.h"



// The entry point the engine calls: broad phase first, then the per-shape tests. On a hit it reports
// where the two met and whether a line was involved (no point when one shape sits inside the other).
bool collision(CMPUT350::CollisionObject& obj1, CMPUT350::CollisionObject& obj2, CMPUT350::Point2D& pnt, bool& lineInvolved);
// Separating axis test for two rectangles, each with its own rotation about its own offset
bool RectangleCollision(const CMPUT350::Rect& obj1, const CMPUT350::Rect& obj2, float r1, float r2, CMPUT350::Point2D o1, CMPUT350::Point2D o2);
// The cheap broad phase: do the two objects' bounding boxes even touch
bool BoundingBoxCollision(CMPUT350::CollisionObject& obj1, CMPUT350::CollisionObject& obj2);
