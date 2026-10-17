#include "CollisionObject.h"
#include "MathUtil.h"



bool collision(CMPUT350::CollisionObject& obj1, CMPUT350::CollisionObject& obj2, CMPUT350::Point2D& pnt, bool& lineInvolved);
bool RectangleCollision(const CMPUT350::Rect& obj1, const CMPUT350::Rect& obj2, float r1, float r2, CMPUT350::Point2D o1, CMPUT350::Point2D o2);
bool BoundingBoxCollision(CMPUT350::CollisionObject& obj1, CMPUT350::CollisionObject& obj2);
