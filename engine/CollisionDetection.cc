#include "CollisionDetection.h"

#include "CollisionObject.h"
#include "MathUtil.h"


static bool Overlaps(const CMPUT350::Rect& a, const CMPUT350::Rect& b) {
    return a.topLeft.x < b.topLeft.x + b.width &&
           b.topLeft.x < a.topLeft.x + a.width &&
           a.topLeft.y < b.topLeft.y + b.height &&
           b.topLeft.y < a.topLeft.y + a.height;
}


inline bool line_circle_collision(const CMPUT350::Line& line, const CMPUT350::Circle& circle, CMPUT350::Point2D& pnt) {

    auto closest_pnt = line.ClosestPoint(circle.center);
    auto distance_sqrd = closest_pnt.DistanceSquared(circle.radius);

    if (distance_sqrd > SquareFloat(circle.radius)) return false;
    // if the square of the distance between closest point and radius is larger than the radius squared, then we DO NOT have a collision


    auto q_sqr = SquareFloat(circle.radius) - distance_sqrd;

    auto distance_p1_p = line.p1.Distance(closest_pnt);

    pnt = closest_pnt + (((line.p1 - closest_pnt) * (1/distance_p1_p)) * sqrt(q_sqr)); 

    return true;


}


inline bool line_rect_collision(const CMPUT350::Line& line, const CMPUT350::Rect& rect, CMPUT350::Point2D& pnt) {

    const auto& x = rect.topLeft.x;
    const auto& y = rect.topLeft.y;

    const auto& width = rect.width;
    const auto& height = rect.height;

    // corners of the rectangle
    const auto topRight = CMPUT350::Point2D(x + width, y);
    const auto& topLeft = rect.topLeft;
    const auto bottomLeft = CMPUT350::Point2D(x, y - height);
    const auto bottomRight = CMPUT350::Point2D(x + width, y - height);


    
    const CMPUT350::Line top_edge(topLeft, topRight);
    if (line.Crosses(top_edge, pnt)) return true;
    

    
    CMPUT350::Line bottom_edge(bottomLeft, bottomRight);
    if (line.Crosses(bottom_edge, pnt)) return true;

    

    
    CMPUT350::Line left_edge(topLeft, bottomLeft);
    if (line.Crosses(left_edge, pnt)) return true;
    
    
    CMPUT350::Line right_edge(topRight, bottomRight);
    if (line.Crosses(right_edge, pnt)) return true;
     
    return false;


}

bool line_collides(const CMPUT350::Line& line, const CMPUT350::CollisionObject& obj, CMPUT350::Point2D& pnt) {
    const auto& shapes = obj.GetShapes();
    const auto obj_offset = obj.GetLocation(); // assuming 'line''s offset has already been applied

    for (const auto& shape : shapes) {
        switch(shape.t) {

            case CMPUT350::ShapeType::kLine: {
                CMPUT350::Line obj_line = shape.shape.line;
                obj_line.p1 += obj_offset;
                obj_line.p2 += obj_offset;
                if (line.Crosses(obj_line, pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                CMPUT350::Circle obj_circle = shape.shape.circle;
                obj_circle.center += obj_offset;
                if(line_circle_collision(line,obj_circle, pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                CMPUT350::Rect obj_rect = shape.shape.rect;
                obj_rect.topLeft += obj_offset;
                if(line_rect_collision(line, obj_rect, pnt)) return true;


            } break;

        }
    } return false;

}

bool circle_rect_collision(const CMPUT350::Circle& circle, const CMPUT350::Rect& rect, CMPUT350::Point2D& pnt) {
    const auto& x = rect.topLeft.x;
    const auto& y = rect.topLeft.y;

    const auto& width = rect.width;
    const auto& height = rect.height;

    // corners of the rectangle
    const auto topRight = CMPUT350::Point2D(x + width, y);
    const auto& topLeft = rect.topLeft;
    const auto bottomLeft = CMPUT350::Point2D(x, y - height);
    const auto bottomRight = CMPUT350::Point2D(x + width, y - height);


    
    const CMPUT350::Line top_edge(topLeft, topRight);
    if (line_circle_collision(top_edge, circle, pnt)) return true;
    

    
    CMPUT350::Line bottom_edge(bottomLeft, bottomRight);
    if (line_circle_collision(bottom_edge, circle, pnt)) return true;

    

    
    CMPUT350::Line left_edge(topLeft, bottomLeft);
    if (line_circle_collision(left_edge, circle, pnt)) return true;
    
    
    CMPUT350::Line right_edge(topRight, bottomRight);
    if (line_circle_collision(right_edge, circle, pnt)) return true;
     
    return false;


}






bool circle_collides(const CMPUT350::Circle& circle, const CMPUT350::CollisionObject& obj, CMPUT350::Point2D& pnt) {

    const auto& shapes = obj.GetShapes();
    const auto obj_offset = obj.GetLocation(); // assuming 'line''s offset has already been applied

    for (const auto& shape : shapes) {
        switch(shape.t) {

            case CMPUT350::ShapeType::kLine: {
                CMPUT350::Line obj_line = shape.shape.line;
                obj_line.p1 += obj_offset;
                obj_line.p2 += obj_offset;
                if(line_circle_collision(obj_line, circle, pnt)) return true;;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                CMPUT350::Circle obj_circle = shape.shape.circle;
                obj_circle.center += obj_offset;
                if (!(circle.center.DistanceSquared(obj_circle.center) > SquareFloat(circle.radius + obj_circle.radius))) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                CMPUT350::Rect obj_rect = shape.shape.rect;
                obj_rect.topLeft += obj_offset;
                if(circle_rect_collision(circle, obj_rect, pnt)) return true;


            } break;
        }
    } return false;



}


bool rect_collides(const CMPUT350::Rect& rect, const CMPUT350::CollisionObject& obj, CMPUT350::Point2D& pnt) {

    const auto& shapes = obj.GetShapes();
    const auto obj_offset = obj.GetLocation(); // assuming 'line''s offset has already been applied

    for (const auto& shape : shapes) {
        switch(shape.t) {

            case CMPUT350::ShapeType::kLine: {
                CMPUT350::Line obj_line = shape.shape.line;
                obj_line.p1 += obj_offset;
                obj_line.p2 += obj_offset;
                if (line_rect_collision(obj_line, rect, pnt)) return true;;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                CMPUT350::Circle obj_circle = shape.shape.circle;
                obj_circle.center += obj_offset;
                if(circle_rect_collision(obj_circle, rect, pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                CMPUT350::Rect obj_rect = shape.shape.rect;
                obj_rect.topLeft += obj_offset;
                if (Overlaps(rect, obj_rect)) return true;


            } break;
        }
    }

    return false;

}


inline bool collision(const CMPUT350::CollisionObject& obj1, const CMPUT350::CollisionObject& obj2, CMPUT350::Point2D& pnt) {

    if (!(Overlaps(obj1.GetBounds(), obj2.GetBounds()))) return false;

    for (const auto& obj1_shape : obj1.GetShapes()) {
        
        switch(obj1_shape.t) {
            case CMPUT350::ShapeType::kLine: {
                if(line_collides(obj1_shape.shape.line, obj2, pnt)) return true;
            } break;

            case CMPUT350::ShapeType::kCircle: {
                if (circle_collides(obj1_shape.shape.circle, obj2, pnt)) return true;
            } break;

            case CMPUT350::ShapeType::kRect: {
                if (rect_collides(obj1_shape.shape.rect, obj2, pnt)) return true;

            } break;
        }
    }

    return false;

}


