#include "CollisionDetection.h"

namespace CMPUT350 {

namespace {

// True when two axis-aligned boxes share area; boxes that only touch at an edge do not count.
bool Overlaps(const Rect &a, const Rect &b) {
    return a.topLeft.x < b.topLeft.x + b.width &&
           b.topLeft.x < a.topLeft.x + a.width &&
           a.topLeft.y < b.topLeft.y + b.height &&
           b.topLeft.y < a.topLeft.y + a.height;
}

// --- The three pair tests that involve a line ---

bool LineCircle(const Line &line, const Circle &circle, Point2D &pnt) {
    // The nearest point on the segment to the centre is inside the circle exactly when they overlap
    Point2D closest = line.ClosestPoint(circle.center);
    if (closest.Distance(circle.center) > circle.radius) return false;
    pnt = closest;
    return true;
}

bool LineRect(const Line &line, const Rect &rect, Point2D &pnt) {
    // The four edges. y grows downward in this engine, so the bottom edge sits at topLeft.y + height.
    Point2D tl = rect.topLeft;
    Point2D tr(tl.x + rect.width, tl.y);
    Point2D bl(tl.x, tl.y + rect.height);
    Point2D br(tl.x + rect.width, tl.y + rect.height);

    return line.Crosses(Line(tl, tr), pnt) ||
           line.Crosses(Line(tr, br), pnt) ||
           line.Crosses(Line(br, bl), pnt) ||
           line.Crosses(Line(bl, tl), pnt);
}

bool CircleRect(const Circle &circle, const Rect &rect, Point2D &pnt) {
    // The nearest point of the rectangle to the centre is the centre clamped onto the box
    float nx = std::max(rect.topLeft.x, std::min(circle.center.x, rect.topLeft.x + rect.width));
    float ny = std::max(rect.topLeft.y, std::min(circle.center.y, rect.topLeft.y + rect.height));
    Point2D nearest(nx, ny);

    if (nearest.Distance(circle.center) > circle.radius) return false;
    pnt = nearest;
    return true;
}

bool CircleCircle(const Circle &a, const Circle &b, Point2D &pnt) {
    Point2D between = b.center - a.center;
    if (between.Distance(Point2D(0.0f, 0.0f)) > a.radius + b.radius) return false;

    // A point on the contact: step from a's centre toward b's centre by a's radius
    between.Normalize();
    pnt = a.center + between * a.radius;
    return true;
}

bool RectRect(const Rect &a, const Rect &b, Point2D &pnt) {
    if (!Overlaps(a, b)) return false;

    // Centre of the shared area
    float left = std::max(a.topLeft.x, b.topLeft.x);
    float top = std::max(a.topLeft.y, b.topLeft.y);
    float right = std::min(a.topLeft.x + a.width, b.topLeft.x + b.width);
    float bottom = std::min(a.topLeft.y + a.height, b.topLeft.y + b.height);
    pnt = Point2D((left + right) * 0.5f, (top + bottom) * 0.5f);
    return true;
}

// --- Moving a local shape into the world, and testing one shape against another ---

Line WorldLine(const Line &l, const Point2D &offset) { return Line(l.p1 + offset, l.p2 + offset); }
Circle WorldCircle(const Circle &c, const Point2D &offset) { return Circle(c.center + offset, c.radius); }
Rect WorldRect(const Rect &r, const Point2D &offset) { return Rect(r.topLeft + offset, r.width, r.height); }

// Both shapes must already be in world space. Symmetric, so the callers can lean on that.
bool ShapeShape(const Shape &a, const Shape &b, Point2D &pnt) {
    switch (a.t) {
        case ShapeType::kLine:
            switch (b.t) {
                case ShapeType::kLine: return a.shape.line.Crosses(b.shape.line, pnt);
                case ShapeType::kCircle: return LineCircle(a.shape.line, b.shape.circle, pnt);
                case ShapeType::kRect: return LineRect(a.shape.line, b.shape.rect, pnt);
            }
            break;
        case ShapeType::kCircle:
            switch (b.t) {
                case ShapeType::kLine: return LineCircle(b.shape.line, a.shape.circle, pnt);
                case ShapeType::kCircle: return CircleCircle(a.shape.circle, b.shape.circle, pnt);
                case ShapeType::kRect: return CircleRect(a.shape.circle, b.shape.rect, pnt);
            }
            break;
        case ShapeType::kRect:
            switch (b.t) {
                case ShapeType::kLine: return LineRect(b.shape.line, a.shape.rect, pnt);
                case ShapeType::kCircle: return CircleRect(b.shape.circle, a.shape.rect, pnt);
                case ShapeType::kRect: return RectRect(a.shape.rect, b.shape.rect, pnt);
            }
            break;
    }
    return false;
}

}  // namespace

bool collision(CollisionObject &obj1, CollisionObject &obj2, Point2D &collisionPoint) {
    // Broad phase: if the world boxes do not touch, no shape pair inside them can either
    if (!Overlaps(obj1.GetBounds(), obj2.GetBounds())) return false;

    const Point2D offset1 = obj1.GetLocation();
    const Point2D offset2 = obj2.GetLocation();

    for (const Shape &local1 : obj1.GetShapes()) {
        for (const Shape &local2 : obj2.GetShapes()) {
            // The shapes live in local space, so each is translated into the world before it is tested
            Shape world1 = local1;
            Shape world2 = local2;
            switch (world1.t) {
                case ShapeType::kLine: world1.shape.line = WorldLine(local1.shape.line, offset1); break;
                case ShapeType::kCircle: world1.shape.circle = WorldCircle(local1.shape.circle, offset1); break;
                case ShapeType::kRect: world1.shape.rect = WorldRect(local1.shape.rect, offset1); break;
            }
            switch (world2.t) {
                case ShapeType::kLine: world2.shape.line = WorldLine(local2.shape.line, offset2); break;
                case ShapeType::kCircle: world2.shape.circle = WorldCircle(local2.shape.circle, offset2); break;
                case ShapeType::kRect: world2.shape.rect = WorldRect(local2.shape.rect, offset2); break;
            }

            if (ShapeShape(world1, world2, collisionPoint)) return true;
        }
    }
    return false;
}

}  // namespace CMPUT350
