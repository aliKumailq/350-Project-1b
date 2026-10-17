#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <cstdint>

#include "MathUtil.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

struct RGBColor {
    uint8_t r, g, b;
    RGBColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

namespace Colors {

const RGBColor red(255, 0, 0);
const RGBColor green(0, 255, 0);
const RGBColor blue(0, 0, 255);
const RGBColor yellow(255, 255, 0);
const RGBColor cyan(0, 255, 255);
const RGBColor magenta(255, 0, 255);
const RGBColor white(255, 255, 255);
const RGBColor black(0, 0, 0);
const RGBColor gray(100, 100, 100);
const RGBColor grey(200, 200, 200);

}  // namespace Colors

// What objects draw through. It holds the window and the font, plus a transform the engine sets to
// the current object's location and rotation, so a draw lands in that object's local space.
class DrawContext {
public:
    DrawContext(std::shared_ptr<sf::RenderWindow> mWindow, std::shared_ptr<sf::Font> font);
    // Text placed with p at its top-left corner
    void DrawText(const std::string &text, int pixelSize, Point2D p ,RGBColor c);
    // Text placed with p at its centre
    void DrawCenteredText(const std::string &text, int pixelSize,Point2D p ,RGBColor c);
    void DrawCircle(Point2D p,float radius,RGBColor c);
    void DrawRect(Rect r, RGBColor c);
    // Empty; nothing in the game draws a framed rectangle
    void FrameRect(Rect r, float width, RGBColor c);
    // A line from `from` to `to`, `width` pixels thick
    void DrawLine(Point2D from, Point2D to, float width, RGBColor c);
    int GetWindowWidth();
    int GetWindowHeight();


private:
    // Builds the sf::Text and does the one transform, so every text draw can share it
    inline sf::Text GetSfText(const std::string &text, int pixelSize, Point2D p, RGBColor c);

    friend class GameEngine;

    // Undo of Transform, so a world point can be brought back into the object's local space
	Point2D ReverseTransform(const Point2D& p);
	Rect Transform(Rect r); // Rotates rectangle and computes new bounding box after
    // Move a local point into the world: rotate about the origin, then add the offset
    Point2D Transform(const Point2D& p);

    // The engine sets these to whatever object it is about to draw
    void SetContextOffset(const Point2D& p);
	void SetContextRotation(float rotation);


    Point2D mOffset; // context offset
    float mRotation;
    float mRotation_cos;
    float mRotation_sin;

    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
};

}  // namespace CMPUT350

#endif  // RENDERTARGET_H
