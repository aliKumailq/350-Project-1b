#include "CollisionObject.h"
#include "MathUtil.h"


bool line_collides(const CMPUT350::Line& line, const CMPUT350::CollisionObject& obj, CMPUT350::Point2D& pnt);


bool circle_collides(const CMPUT350::Circle& circle, const CMPUT350::CollisionObject& obj, CMPUT350::Point2D& pnt);

bool rect_collides(const CMPUT350::Rect& rect, const CMPUT350::CollisionObject& obj, CMPUT350::Point2D& pnt);

bool collision(const CMPUT350::CollisionObject& obj1, const CMPUT350::CollisionObject& obj2, CMPUT350::Point2D& pnt);