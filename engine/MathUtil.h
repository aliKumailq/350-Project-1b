#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <algorithm>
#include <cmath>
#include <iostream>

static inline float SquareFloat(const float& f) { return f * f; }

namespace CMPUT350 {

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}

    inline double DistanceSquared(const Point2D& other) const {
        return SquareFloat(x - other.x) + SquareFloat(y - other.y);
    }

    double Distance(const Point2D& other) const {
        return sqrt(DistanceSquared(other));
    }
    Point2D operator+(const Point2D& other) const { return Point2D(x + other.x, y + other.y); }
    Point2D operator+(const float& other) const { return Point2D(x + other, y + other); }
    Point2D operator-(const Point2D& other) const { return Point2D(x - other.x, y - other.y); }
    Point2D operator-(const float& other) const { return Point2D(x - other, y - other); }
    Point2D operator*(const float& scalar) const { return Point2D(x * scalar, y * scalar); }
    Point2D& operator+=(const float& scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    Point2D& operator+=(const Point2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Point2D& operator-=(const Point2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    bool operator==(const Point2D& other) const { return (x == other.x && y == other.y); }
    Point2D& operator*=(const int& scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Point2D& operator/=(
        float scalar) {  // int gets truncated and results in zeros/infinities, so we use float
        x /= scalar;
        y /= scalar;
        return *this;
    }
    float operator*(const Point2D& other) const { return (x * other.x) + (y * other.y); }
    float Dot(Point2D b) const { return (*this) * b; }
    float Cross(Point2D b) const {  // spec lists the member form, so the static forwards here
        return (x * b.y) - (y * b.x);
    }
    static float Dot(Point2D a, Point2D b) {  // not needed until 1b
        return a * b;
    }
    static float Cross(Point2D a, Point2D b) {  // not needed until 1b
        return a.Cross(b);
    }

    void Normalize() {
        // Normalizes this vector to unit length
        float lengthsquared = ((Dot(*this)));
        if (lengthsquared != 0.0f) {
            float inverse = 1 / sqrt(lengthsquared);
            x *= inverse;
            y *= inverse;
        }
        // We use the idea from quake by finding the squared length, checking for the zero edge
        // case, then using the inverse once to find the scale factor needed to normalize both x and
        // y
    }
};

static std::ostream& operator<<(std::ostream& os, const Point2D& p) {
    os << "(" << p.x << ", " << p.y << ")";  // Return os by reference not void so we can chain our
                                             // outputs since void breaks << chaining
    return os;
}

static Point2D operator*(float number, const Point2D& rhs) {
    return Point2D(rhs.x * number, rhs.y * number);
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const { return p1.Distance(p2); }
    Point2D ClosestPoint(const Point2D& p) const {
        // Finds the closest point on a given segment, not the infinite line

        Point2D direction = p2 - p1;
        Point2D toPoint = p - p1;
        float lengthSquared = direction.Dot(direction);
        if (lengthSquared == 0.0f) {  // guard against divide by 0
            return p1;
        }

        float projected = toPoint.Dot(direction);
        projected = projected / lengthSquared;

        if (projected < 0.0f) {
            projected = 0.0f;
        } else if (projected > 1.0f) {
            projected = 1.0f;
        }

        Point2D closest = p1 + (projected * direction);

        return closest;
    }
    bool Crosses(Line other, Point2D& crossingPoint) const {
        // Turn each segment into a direction vector; If we start at p1, which way and how far do we
        // travel to reach p2?
        Point2D r = p2 - p1;
        Point2D s = other.p2 - other.p1;

        // Cross the two directions. If this is 0, the directions are parallel, so there is no
        // intersection point for us to solve for.
        float denom = Point2D::Cross(r, s);

        if (denom == 0.0f) {
            return false;
        }

        // Arrow from MY starting point to THEIR starting point. This tells us how separated the two
        // journeys are at the beginning.
        Point2D offset = other.p1 - p1;

        // For this part, just to explain the algorithm, we basically have p1 + t*r = other.p1 +
        // u*s, which finds where in the journey we will both cross given our starting points and a
        // given percentage of the journey (t and u), when we rearrange this we get t*r - u*s =
        // other.p1 - p1 or basically t*r - u*s = offset. Now we have an equation to find the
        // meeting point since we can calculate offset, but currently both u and t are mixed
        // together, so to isolate it we cross everything with s, because, anything crossed with
        // itself is 0 which removed the u*s term and leaves us t * Cross(r, s) = Cross(offset, s),
        // so we can solve t by rearranging to t = Cross(offset, s) / Cross(r, s), then repeat by
        // crossing with r to erase our direction and isolate theirs

        // Find how far along MY segment the meeting point would be. Crossing with THEIR direction
        // effectively removes their movement from the equation, leaving us with my progress: t.
        float t = Point2D::Cross(offset, s) / denom;

        // Find how far along THEIR segment the meeting point would be. Same idea, but cross with MY
        // direction to isolate their progress: u.
        float u = Point2D::Cross(offset, r) / denom;

        // t and u are percentages of each journey:
        //   0   = starting point
        //   0.5 = halfway
        //   1   = ending point

        // Outside [0, 1] means they meet on the infinite line, not within our segment, so for our
        // purposes they don't meet.
        if (t < 0.0f || t > 1.0f || u < 0.0f || u > 1.0f) {
            return false;
        }

        // We now know the intersection happens t-of-the-way along my segment. Start at p1 and walk
        // t * r to get the actual point.
        crossingPoint = p1 + (t * r);

        return true;
    }
};

static std::ostream& operator<<(std::ostream& os, const Line& l) {
    os << "Line(" << l.p1 << ", " << l.p2 << ")";  //  this reads like the constructors but  (0, 0)
                                                   //  is a comma expression, not a Point2D
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

static std::ostream& operator<<(std::ostream& os, const Circle& c) {
    os << "Circle(" << c.center << ", " << c.radius << ")";
    return os;
}

struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    // float to match the spec, and so widths aren't truncated at the call site
    Rect(Point2D tl = {0, 0}, float w = 0, float h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect& operator|=(const Rect& other) {
        float left = topLeft.x;
        float top = topLeft.y;
        float right = topLeft.x + width;
        float bottom = topLeft.y + height;

        float otherleft = other.topLeft.x;
        float othertop = other.topLeft.y;
        float otherright = other.topLeft.x + other.width;
        float otherbottom = other.topLeft.y + other.height;

        float newleft = std::min(left, otherleft);
        float newtop = std::min(top, othertop);
        float newright = std::max(right, otherright);
        float newbottom = std::max(bottom, otherbottom);

        topLeft.x = newleft;
        topLeft.y = newtop;
        width = newright - topLeft.x;
        height = newbottom - topLeft.y;

        return *this;
    }
    Rect& operator|=(const Point2D& other) {
        // Same as above but we expand to include one point instead of a whole rectangle
        float left = topLeft.x;
        float top = topLeft.y;
        float right = topLeft.x + width;
        float bottom = topLeft.y + height;

        float newleft = std::min(left, other.x);
        float newtop = std::min(top, other.y);
        float newright = std::max(right, other.x);
        float newbottom = std::max(bottom, other.y);

        topLeft.x = newleft;
        topLeft.y = newtop;
        width = newright - topLeft.x;
        height = newbottom - topLeft.y;

        return (*this);
    }
    Rect& operator|=(const Line& other) {
        // Makes a rectangle big enough to encompass our line
        // if we have Rect r; and Point2D p; then r |= p; means we expand rectangle r so that p is
        // inside of it.

        *this |= other.p1;  // expand to encompass point 1
        *this |= other.p2;  // expand to encompass point 2

        return *this;
    }
    Rect& operator&=(const Rect& other) {
        // we calculate the shaded region of both rectangles overlap
        // for a &= b; we mutate a so that it becomes the intersection of a and b
        float left = topLeft.x;
        float top = topLeft.y;
        float right = topLeft.x + width;
        float bottom = topLeft.y + height;

        float otherleft = other.topLeft.x;
        float othertop = other.topLeft.y;
        float otherright = other.topLeft.x + other.width;
        float otherbottom = other.topLeft.y + other.height;

        float newleft = std::max(left, otherleft);
        float newtop = std::max(top, othertop);
        float newright = std::min(right, otherright);
        float newbottom = std::min(bottom, otherbottom);
        if (newright < newleft || newbottom < newtop) {
            width = 0;
            height = 0;
            return (*this);
        }
        topLeft.x = newleft;
        topLeft.y = newtop;
        width = newright - topLeft.x;
        height = newbottom - topLeft.y;

        return *this;
    }
    Rect& operator+=(const Point2D& other) {
        topLeft += other;  // changing the offset
        return *this;
    }
    Rect operator+(const Point2D& other) const {
        auto newRect = *this;
        newRect += other;
        return newRect;
    }
    void Inset(int inset) {
        // Shrinks the Rectangle inward from every side by the same amount

        float left = topLeft.x;
        float top = topLeft.y;

        topLeft.x = left + inset;
        topLeft.y = top + inset;
        width = width - (2 * inset);
        height = height - (2 * inset);

        // This can return negative dimensions if inset is larger than 1/2 width or height.
        // Potential edge case to consider later.
    }
    bool IsInside(const Point2D& p) const {
        // Returns True if a box is inside another box else false
        return (p.x >= topLeft.x && p.x <= topLeft.x + width && p.y >= topLeft.y &&
                p.y <= topLeft.y + height);
    }
};

static std::ostream& operator<<(std::ostream& os, const Rect& r) {
    os << "Rect(" << r.topLeft << ", " << r.width << ", " << r.height << ")";
    return os;
}

// The shapes a collision object reports; all in the object's local space
enum class ShapeType {
    kLine, kCircle, kRect
};

union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect; // Axis-aligned rectangle
};

struct Shape {
    Shape(const Line &l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle &c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect &r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};

}  // namespace CMPUT350

#endif  // MATHUTIL_H
