#ifndef MATH_HELFERS

#include "MathUtil.h"
#include "CollisionObject.h"



namespace {
namespace RectHelpers {
    using namespace CMPUT350;
    struct RectangleVerticies {

        RectangleVerticies(const Rect& rect) : tl(rect.topLeft), tr(rect.topLeft.x + rect.width, rect.topLeft.y),
        bl(rect.topLeft.x, rect.topLeft.y + rect.height), br(rect.topLeft.x + rect.width, rect.topLeft.y + rect.height) {}

        RectangleVerticies() {}

        std::vector<Point2D> GetAllVert() const {
            return {tl, tr, bl, br};
        }

        Point2D tl, tr, bl, br;

    };

    struct RectangleEdges {
        Line top, bottom, left, right;


    };

    static inline RectangleEdges GetRectEdges(const RectangleVerticies& verticies) {
        struct RectangleEdges edges;
        edges.top = Line(verticies.tl, verticies.tr);
        edges.bottom = Line(verticies.bl, verticies.br);
        edges.right = Line(verticies.tr, verticies.br);
        edges.left = Line(verticies.tl, verticies.bl);
        return edges;
    };

    static inline RectangleVerticies GetRectVerticies(const Rect& rect) {
        struct RectangleVerticies vert;
        vert.tl = rect.topLeft;
        vert.tr = Point2D(rect.topLeft.x + rect.width, rect.topLeft.y);
        vert.bl = Point2D(rect.topLeft.x, rect.topLeft.y + rect.height);
        vert.br = Point2D(rect.topLeft.x + rect.width, rect.topLeft.y + rect.height);
        return vert;
        

    }



    static inline void TransformVerticies(RectangleVerticies& verticies, const Point2D& offset, const double& rotation, bool reverse = false) {
        auto cos_d = cosf(rotation);
        auto sin_d = sinf(rotation);
        auto transform = [&cos_d, &sin_d, &offset, &reverse](Point2D& p) -> void {
            auto x = reverse ? (p.x * cos_d + p.y * sin_d) : (p.x * cos_d - p.y * sin_d);
            auto y = reverse ? (p.y * cos_d - p.x * sin_d) : (p.x * sin_d + p.y * cos_d);
            p = Point2D(x,y) + offset;
        }; 

        transform(verticies.bl);
        transform(verticies.br);
        transform(verticies.tl);
        transform(verticies.tr);
        return;


    }

    using axis_range = std::pair<double, double>;
    using Vector2D = Point2D;

    axis_range ProjectionRange(const std::vector<Point2D>& verts, const Vector2D& axis) {
        assert(!(verts.empty()));
        double min = verts[0] * axis;
        double max = min;

        for (auto& vert : verts) {
            auto dot = vert.Dot(axis);
            if (dot > max) max = dot;
            else if (dot < min) min = dot;

        } return {min, max};
        
    }

    inline bool OverlappingRanges(const axis_range& a1, const axis_range& a2) {
        return std::max(a1.first, a2.first) <= std::min(a1.second, a2.second);
    }

}

}



#endif