#include <algorithm>
#include <cmath>
#include <memory>
#include "MathUtil.h"
#include "CollisionObject.h"
#include "CollisionDetection.h"
#include "RectHelpers.h"
using namespace RectHelpers;





bool RectangleCollision(const CMPUT350::Rect& obj1, const CMPUT350::Rect& obj2, float r1, float r2, Point2D o1, Point2D o2) {

    RectangleVerticies rvert1(obj1);
    TransformVerticies(rvert1, o1, r1);

    RectangleVerticies rvert2(obj2);
    TransformVerticies(rvert2, o2, r2);


    auto verts1 = rvert1.GetAllVert();
    auto verts2 = rvert2.GetAllVert();

    RectangleEdges edges1 = GetRectEdges(rvert1);
    RectangleEdges edges2 = GetRectEdges(rvert2);

    std::vector<Line> edges = {
        edges1.bottom, edges2.bottom,
        edges1.left, edges2.left,
        edges1.right, edges2.right,
        edges1.top, edges2.top
     };

    for (const auto& line : edges) {
        Point2D axis = line.p2 - line.p1;
        axis = {-1 * axis.y, axis.x}; // taking the perpendicular of each axis
        auto rect1ProjRange = ProjectionRange(verts1, axis);
        auto rect2ProjRange = ProjectionRange(verts2, axis);
        if (!OverlappingRanges(rect1ProjRange, rect2ProjRange)) return false;



    } return true;

}


bool BoundingBoxCollision(CMPUT350::CollisionObject &obj1, CMPUT350::CollisionObject &obj2) {
    return RectangleCollision(obj1.GetBounds(), obj2.GetBounds(), obj1.GetRotation(), obj2.GetRotation(), obj1.GetLocation(), obj2.GetLocation());
}