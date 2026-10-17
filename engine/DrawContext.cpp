#include "DrawContext.h"

#include <cmath>

#include "Helpers.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Angle.hpp>

static inline sf::Vector2f GetSfVec(const CMPUT350::Point2D& pnt) {
    return sf::Vector2f(pnt.x, pnt.y);
}

static inline sf::Color GetSfColor(const CMPUT350::RGBColor& c) { return sf::Color(c.r, c.g, c.b); }

namespace CMPUT350 {

// Start from no transform: rotation of zero and an offset at the origin
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window),
      mFont(font),
      mOffset(Point2D(0, 0)),
      mRotation(0),
      mRotation_cos(1),
      mRotation_sin(0) {}

// Build the sf::Text and do the transform here, so the two text draws can hand over a raw point
inline sf::Text DrawContext::GetSfText(const std::string& text, int pixelSize, Point2D p,
                                       RGBColor c) {
    
    sf::Text my_text(*mFont, text, pixelSize);
    my_text.setFillColor(sf::Color(c.r, c.g, c.b));
    my_text.setPosition(GetSfVec(Transform(p)));
    return my_text;
}

void DrawContext::DrawCenteredText(const std::string& text, int pixelSize,Point2D p ,RGBColor c) {
    // GetSfText is the one place that transforms a point, so the raw p goes in and is shifted once
    auto my_text = GetSfText(text, pixelSize, p, c);
    auto text_bounding_box = my_text.getLocalBounds();

    my_text.setOrigin(text_bounding_box.position + (text_bounding_box.size / 2.0f));
    // The texts local coordinate system goes beyoned an sf::Text's bounding box.
    // The origin of an sf::Text WILL (by default) be (0,0)
    // But that marks the origin of an sf::Texts local origin coordinate system, not the top left
    // corner of thebounding box So we have to offset FROM the top left of the bounding box to its
    // center.

    mWindow->draw(my_text);
}

void DrawContext::DrawText(const std::string& text, int pixelSize, Point2D p, RGBColor c) {
    // Same as the centred draw: GetSfText does the transforming, so p is handed over untouched
    mWindow->draw(GetSfText(text, pixelSize, p, c));
}

// Drawn with its origin at the centre, so p is the circle's centre, not a corner
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape circle(
        radius);  // FIXME: Should probably throw some exception if the radius is 0.


    circle.setPosition(GetSfVec(Transform(p)));
    circle.setFillColor(sf::Color(c.r, c.g, c.b));

    auto localBounds = circle.getLocalBounds();
    auto new_origin = localBounds.position + (localBounds.size / 2.0f);
    // Setting the origin as the center of the circle.
    // Fixing this after implementing DrawCenterText

    circle.setOrigin(new_origin);

    mWindow->draw(circle);
}

void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape rectangle;

    // The rect arrives in the object's local space. The spec says the location is the point rotations
    // happen around, so instead of placing the top-left corner and spinning the shape about it, we put
    // the shape's own origin on the rect's centre and rotate around that. The centre is then moved to
    // the transformed centre, so an object whose box is centred on the origin pivots on its location.
    const Point2D centre(r.topLeft.x + (r.width / 2.0f), r.topLeft.y + (r.height / 2.0f));

    rectangle.setSize(sf::Vector2f(r.width, r.height));
    rectangle.setOrigin(sf::Vector2f(r.width / 2.0f, r.height / 2.0f));
    rectangle.setPosition(GetSfVec(Transform(centre)));
    rectangle.setRotation(sf::radians(mRotation));
    rectangle.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(rectangle);
}

// Nothing in the game draws a framed rectangle, so this is left empty
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    // To get the vector pointing SIDEWAYS across the line, rotate that direction 90 degrees: (dx,
    // dy) -> (-dy, dx) The old code used (dx, -dy), which only mirrored the direction instead of
    // making it perpendicular. That produced a diamond shape instead of a proper rectangle.
    from = Transform(from) ;
    to = Transform(to);

    


    Helpers::Vector2D end(-(to.y - from.y), (to.x - from.x));
    end.Normalize();
    end =
        end *
        (width /
         2.0f);  // float multiply, since operator*= takes an int and would truncate thin lines away

    sf::ConvexShape line;

    auto relative = to - from;  // vector from 'from' to 'ti'

    line.setPointCount(4);

    line.setPoint(0, GetSfVec(end));
    line.setPoint(1, GetSfVec(relative + end));

    line.setPoint(2, GetSfVec(relative - end));
    line.setPoint(3, GetSfVec(end * -1));

    line.setFillColor(GetSfColor(c));
    line.setPosition(GetSfVec(from));

    mWindow->draw(line);
}

// The window's width in pixels
int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

// The window's height in pixels
int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

// The engine sets this to the object it is about to draw
void DrawContext::SetContextOffset(const Point2D& p) { mOffset = p; }

// Rotation is stored as an angle for draws and as its sin and cos for the point transforms
void DrawContext::SetContextRotation(float rotation) {
    mRotation = rotation;
    mRotation_cos = cosf(rotation);
    mRotation_sin = sinf(rotation);
    return;
}

static inline Point2D TransformPoint(const Point2D& p, const float& cos, const float& sin,
                                     const bool reverse = false) {
    // rotates p relative to its origin

    const float x = reverse ? (p.x * cos + p.y * sin) : (p.x * cos - p.y * sin);
    const float y = reverse ? (p.y * cos - p.x * sin) : (p.x * sin + p.y * cos);
    return Point2D(x, y);
}

// Local to world: rotate about the origin, then add the offset
Point2D DrawContext::Transform(const Point2D& point) {
    return TransformPoint(point, mRotation_cos, mRotation_sin) += mOffset;
}

// World back to local: take the offset off first, then un-rotate
Point2D DrawContext::ReverseTransform(const Point2D& point) {
    return TransformPoint(point - mOffset, mRotation_cos, mRotation_sin, true);
}
Rect DrawContext::Transform(Rect r) {
    // changes the location of r.topLeft with respect to the center after a rotation.

    const float center_x = r.topLeft.x + (r.width / 2);
    const float center_y = r.topLeft.y - (r.height / 2);
    const Point2D rectangle_center(center_x, center_y);
    r.topLeft = TransformPoint(r.topLeft - rectangle_center, mRotation_cos, mRotation_sin) +
                rectangle_center;
    return r;
}

}  // namespace CMPUT350
