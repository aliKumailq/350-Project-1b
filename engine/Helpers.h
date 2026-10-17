#ifndef HELPERS
#define HELPERS

#include "MathUtil.h"
#include "CollisionObject.h"



inline namespace Helpers {
    using namespace CMPUT350;
    struct RectangleVerticies {

        // The four corners worked out from a rect's top-left and its size
        RectangleVerticies(const Rect& rect) : tl(rect.topLeft), tr(rect.topLeft.x + rect.width, rect.topLeft.y),
        bl(rect.topLeft.x, rect.topLeft.y + rect.height), br(rect.topLeft.x + rect.width, rect.topLeft.y + rect.height) {}

        // Leaves the corners unset, to be filled in by hand
        RectangleVerticies() {}

        // The corners as a list, for the loops that project them onto an axis
        std::vector<Point2D> GetAllVert() const {
            return {tl, tr, bl, br};
        }

        Point2D tl, tr, bl, br;

    };

    struct RectangleEdges {
        Line top, bottom, left, right;


    };

    // The four sides of a rectangle, as lines, from its corners
    static inline RectangleEdges GetRectEdges(const RectangleVerticies& verticies) {
        struct RectangleEdges edges;
        edges.top = Line(verticies.tl, verticies.tr);
        edges.bottom = Line(verticies.bl, verticies.br);
        edges.right = Line(verticies.tr, verticies.br);
        edges.left = Line(verticies.tl, verticies.bl);
        return edges;
    };

     // The four corners of a rect, from its top-left and size
     inline RectangleVerticies GetRectVerticies(const Rect& rect) {
        struct RectangleVerticies vert;
        vert.tl = rect.topLeft;
        vert.tr = Point2D(rect.topLeft.x + rect.width, rect.topLeft.y);
        vert.bl = Point2D(rect.topLeft.x, rect.topLeft.y + rect.height);
        vert.br = Point2D(rect.topLeft.x + rect.width, rect.topLeft.y + rect.height);
        return vert;
        

    }

     // Rotate a point about the origin, then add the offset. With reverse set it runs the other way
     // round, taking the offset back off before un-rotating.
     inline CMPUT350::Point2D TransformPoint(const CMPUT350::Point2D& pnt, const CMPUT350::Point2D& offset, const float& cos, const float& sin, const bool& reverse = false) {

        auto newPoint = reverse ? pnt - offset : pnt; // if reverse is on, then we have to strip away the offset before reversing the rotation
        const auto x = reverse ? (newPoint.x * cos + newPoint.y * sin) : (newPoint.x * cos - newPoint.y * sin);
        const auto y = reverse ? (newPoint.y * cos - newPoint.x * sin) : (newPoint.x * sin + newPoint.y * cos);
        newPoint.x = x;
        newPoint.y = y;
        return reverse ? newPoint : newPoint += offset;

    }

     // The same transform, taking an angle instead of a sin and cos
     inline CMPUT350::Point2D TransformPoint(const CMPUT350::Point2D& pnt, const CMPUT350::Point2D& offset, const float& rotate, const bool& reverse = false) {
        const auto cos = cosf(rotate);
        const auto sin = sinf(rotate);
        return TransformPoint(pnt, offset, cos, sin, reverse);

    }




     // Transform all four corners of a rectangle
     inline void TransformVerticies(RectangleVerticies& verticies, const Point2D& offset, const double& rotation, bool reverse = false) {

        auto transform = [&](Point2D& p) -> void {
            p = TransformPoint(p, offset, rotation, reverse);
        }; 

        transform(verticies.bl);
        transform(verticies.br);
        transform(verticies.tl);
        transform(verticies.tr);
        return;


    }

    using axis_range = std::pair<double, double>;
    using Vector2D = Point2D;

     // How far a shape reaches along one axis: the min and max of its corners' projections on it
     inline axis_range ProjectionRange(const std::vector<Point2D>& verts, const Vector2D& axis) {
        assert(!(verts.empty()));
        double min = verts[0] * axis;
        double max = min;

        for (auto& vert : verts) {
            auto dot = vert.Dot(axis);
            if (dot > max) max = dot;
            else if (dot < min) min = dot;

        } return {min, max};
        
    }

     // Two ranges overlap when the later start has not passed the earlier end
     inline bool OverlappingRanges(const axis_range& a1, const axis_range& a2) {
        return std::max(a1.first, a2.first) <= std::min(a1.second, a2.second);
    }

    // Is the range s wholly inside the range S
    inline bool SubsetOf(const axis_range& s, const axis_range& S) {
        return s.first >= S.first && s.second <= S.second;
    }

    // The circle fits inside when its centre is at least a radius clear of every side
    inline bool CircleInsideRectangle(const Circle& circle, const RectangleVerticies& rect) {
        const auto& x = circle.center.x;
        const auto& y = circle.center.y;
        const auto& r = circle.radius;

        auto horizontal_bounds = [&]() -> bool {
            return rect.tl.x + r <= x && x <= rect.tr.x - r;
        };

        auto vertical_bounds = [&]() -> bool {
            return rect.tl.y + r <= y && y <= rect.bl.y;
        };

        return horizontal_bounds() && vertical_bounds();

        
    }

    // The whole line lies inside the rectangle, checked on the x range and then the y range
    inline bool LineInsideRectangle(const Line& line, const RectangleVerticies& rect) {
        axis_range lineXRange = line.p1.x < line.p2.x ? axis_range(line.p1.x, line.p2.x) : axis_range(line.p2.x, line.p1.x);
        axis_range rectXRange(rect.tl.x, rect.tr.x);

        axis_range lineYRange = line.p1.y < line.p2.y ? axis_range(line.p1.y, line.p2.y) : axis_range(line.p2.y, line.p1.y);
        axis_range rectYRange(rect.tl.y, rect.bl.y);

        return SubsetOf(lineXRange, rectXRange) && SubsetOf(lineYRange,rectYRange);



        


    }

        struct Transformation {

        // A transform kept as its offset plus the sin and cos of the rotation, so the trig is done once
        Transformation(const Point2D& pnt, const float& r) : offset(pnt), rotation(r), cos(cosf(rotation)), sin(sinf(rotation)) {}

        // Apply the transform to a point, or undo it when reverse is set
        inline Point2D transform(const Point2D& p_, bool reverse = false) const  {
            return Helpers::TransformPoint(p_, offset, cos, sin, reverse);
        }

        CMPUT350::Point2D offset;
        float rotation;
        float cos;
        float sin;

    };

    struct ShapeTransfer{
        
        // A shape paired with the transform that places it in the world
        ShapeTransfer(const Point2D& pnt, const float& r, const Shape& shape) : transformation(pnt, r), shape(shape) {}

        Transformation transformation;
        Shape shape;

    };

    // Move the circle's centre into the world
    inline void ShapeTransferCircle(ShapeTransfer& circle) {
        assert(circle.shape.t == CMPUT350::ShapeType::kCircle);
        circle.shape.shape.circle.center = circle.transformation.transform(circle.shape.shape.circle.center);
    }

    // Move both of the line's ends into the world
    inline void ShapeTransferLine(ShapeTransfer& line) {
        assert(line.shape.t == CMPUT350::ShapeType::kLine);
        line.shape.shape.line.p1 = line.transformation.transform(line.shape.shape.line.p1);
        line.shape.shape.line.p2 = line.transformation.transform(line.shape.shape.line.p2);
        return;
    }

}





#endif