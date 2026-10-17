#include "CollisionDetection.h"
#include "CollisionObject.h"
#include "MathUtil.h"
#include "Helpers.h"

using namespace Helpers;

static inline bool line_circle_collision(const ShapeTransfer& line_, const ShapeTransfer& circle_, CMPUT350::Point2D& pnt, bool& intersection) {
    auto line__ = line_;
    auto circle__ = circle_;

    ShapeTransferCircle(circle__);
    ShapeTransferLine(line__);


    auto& line = line__.shape.shape.line;
    auto& circle = circle__.shape.shape.circle;




    auto closest_pnt = line.ClosestPoint(circle.center);
    auto distance_sqrd = closest_pnt.DistanceSquared(circle.center);

    auto too_far = [&circle](const Point2D& p) -> bool {
        return p.DistanceSquared(circle.center) > SquareFloat(circle.radius);
    };

    if (!too_far(distance_sqrd)) { // not crossing the circle.
        intersection = false;
        return false;
    }

    if (!too_far(line.p1) && !too_far(line.p2)) { // whole line is inside the circle.
        intersection = false;
        return true;
    }

    
    auto segment = line.p2 - line.p1;
    auto segment_len = line.p2.Distance(line.p1);

    auto AP = line.p1 - circle.center;
    // A == line.p1
    // P == center of circle

    auto first_term = segment * segment;
    // first_term = AB * AB, where B == line.p2

    auto second_term = (segment * AP) * 2;
    auto third_term = (AP * AP - SquareFloat(circle.radius));
    auto dis = (SquareFloat(second_term) - 4 * first_term * third_term);
    // discriminent

    /*
    
    Given a line: P1 + (P2 - P1)t, where t is between 0 and 1, there are at most 2 values of t such that this line intersects with the circle.
    Derive from the fact that if there is a t such that P1 + (P2 - P1)t intercepts the circle then we have:
                    || (P1 + (P2 - P1)t) - circle.center || = radius
                    ((P1 + (P2 - P1)t) - circle.center) *  ((P1 + (P2 - P1)t) - circle.center) = r^2
                    // At this point we use linear algebra to find t, which is a quadratic equation.


                    // More convienient and robust then using pythagoreas with the vectors.

    */

    if (dis < 0) {
        intersection = false;
        return false;
    }

    auto t1 = (-second_term - sqrt(dis)) / (2*first_term);
    auto t2 = (-second_term + sqrt(dis)) / (2*first_term);
    auto valid = [](const double& t) {
        return !(t < 0 || t > 1);
    }; // use the smallest (valid) t. Since a smaller t is closer to P1.

    auto point = [&line, &segment](const double& t) {
        return line.p1 + t * (segment);
    };

    if (valid(t1) && !valid(t2)) {
        intersection = true;
        pnt = point(t1);
        return true;
    } else if (!valid(t1) && valid(t2)) {
        intersection = true;
        pnt = point(t2);
        return true;
    } else if ((valid(t1)) && valid(t2)) {
        intersection = true;
        pnt = point(std::min(t1,t2));
        return true;
    } else {
        intersection = false;
        return false;
    }




}


static inline bool line_rect_collision(const ShapeTransfer& line_, const ShapeTransfer& Rect ,CMPUT350::Point2D& pnt,bool& intersection) {
    // similar to the circle. Treat every edge like a line and then see if the line is inside of the rectangle itself

    assert(Rect.shape.t == CMPUT350::ShapeType::kRect);

    auto line__ = line_;
    ShapeTransferLine(line__);
    auto& line = line__.shape.shape.line;

    auto verticies = Helpers::RectangleVerticies(Rect.shape.shape.rect);
    Helpers::TransformVerticies(verticies, Rect.transformation.offset, Rect.transformation.rotation);

    auto edges = Helpers::GetRectEdges(verticies);
    if (line.Crosses(edges.top,pnt)) return true;
    if (line.Crosses(edges.bottom, pnt)) return true;
    if (line.Crosses(edges.left, pnt)) return true;
    if (line.Crosses(edges.right, pnt)) return true;

    auto old_line = line_.shape.shape.line;
    auto lp1 = Helpers::TransformPoint(old_line.p1, line_.transformation.offset, line_.transformation.rotation);
    auto lp2 = Helpers::TransformPoint(old_line.p2, line_.transformation.offset, line_.transformation.rotation);
    lp1 -= Rect.transformation.offset;
    lp2 -= Rect.transformation.offset; // now these are the two points of the line with respect to the rectangle.

    lp1 = Helpers::TransformPoint(lp1, Point2D(0,0), - Rect.transformation.rotation);
    lp2 = Helpers::TransformPoint(lp2, Point2D(0,0), - Rect.transformation.rotation);
    // undoing the rectangle rotation so that we can compare these points with the axis alligned version of the rectangle.

    auto rect_verticies = Helpers::RectangleVerticies(Rect.shape.shape.rect);
    if (Helpers::LineInsideRectangle(Line(lp1,lp2), rect_verticies)) {
        intersection = false;
        return true; // if the line IS inside of the rectangle entirely, then we DO have a collision... but not one where you draw a point.
    } else return false;



}

static inline bool line_collides(const ShapeTransfer& shape1, const ShapeTransfer& shape2, CMPUT350::Point2D& pnt,bool& intersection) {
    assert(shape1.shape.t == CMPUT350::ShapeType::kLine);
    // handles all cases for what kind of shape shape2 is. given that shape1 is a line.



        switch(shape2.shape.t) {
            case CMPUT350::ShapeType::kLine: {
                auto line1 = shape1;
                ShapeTransferLine(line1);

                auto line2 = shape2;
                ShapeTransferLine(line2); // transform both lines to absolute coords and see if they cross

            
                if (line1.shape.shape.line.Crosses(line2.shape.shape.line, pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                if(line_circle_collision(shape1,shape2, pnt,intersection)) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                if(line_rect_collision(shape1, shape2,pnt,intersection)) return true;


            } break;

        }
    
    return false;

}

static inline bool circle_rect_collision(const ShapeTransfer& circle, const ShapeTransfer& rect ,CMPUT350::Point2D& pnt,bool& intersection) {
    assert(rect.shape.t == CMPUT350::ShapeType::kRect);

    auto verticies = Helpers::RectangleVerticies(rect.shape.shape.rect);
    const auto edges = Helpers::GetRectEdges(verticies);

    // treating each edge like a line and seeing if the circle collides with them.
    auto top = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.top};
    auto bottom = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.bottom};
    auto right = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.right};
    auto left = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.left}; 

    
    if (line_circle_collision(top,circle,pnt,intersection)) return !(intersection = false);
    if (line_circle_collision(bottom,circle, pnt,intersection)) return !(intersection = false);
    if (line_circle_collision(left, circle,pnt,intersection)) return !(intersection = false);
    if (line_circle_collision(right, circle,pnt,intersection)) return !(intersection = false);

    intersection = false; // at this point, the circle is either not touching the rectangle at all... or its inside of the rectangle. 

    // Have to check to see if the circle is INSIDE of the rectangle
    auto transformed_circle = circle.shape.shape.circle;
    transformed_circle.center = circle.transformation.offset - rect.transformation.offset; // transformed_center is now with respect to the rectangle.
    transformed_circle.center = Helpers::TransformPoint(transformed_circle.center, rect.transformation.offset, -rect.transformation.rotation, false);
    Helpers::TransformVerticies(verticies, rect.transformation.offset, 0);
    return (Helpers::CircleInsideRectangle(transformed_circle, verticies)) ;

    
    

}






static inline bool circle_collides( const ShapeTransfer&circle, const ShapeTransfer& shape, CMPUT350::Point2D& pnt,bool& intersection) {

    // handles all cases for what shape the other shape is


        switch(shape.shape.t) {

            case CMPUT350::ShapeType::kLine: {

                if(line_circle_collision(shape, circle, pnt,intersection)) return true;;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                auto circle2 = shape; // punch me in the face
                auto circle1 = circle;
                ShapeTransferCircle(circle1);
                ShapeTransferCircle(circle2);

                if (!(circle1.shape.shape.circle.center.DistanceSquared(circle2.shape.shape.circle.center) > SquareFloat(circle.shape.shape.circle.radius + circle2.shape.shape.circle.radius))) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                if(circle_rect_collision(circle, shape, pnt,intersection)) return true;


            } break;
        }
    
    
    return false;



}


static inline bool rect_collides(const ShapeTransfer&rect, const ShapeTransfer& shape , CMPUT350::Point2D& pnt,bool& intersection) {
    

        switch(shape.shape.t) {

            case CMPUT350::ShapeType::kLine: {

                if (line_rect_collision(shape, rect,pnt,intersection)) return true;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                if(circle_rect_collision(shape, rect ,pnt,intersection)) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                if (RectangleCollision(rect.shape.shape.rect, shape.shape.shape.rect, 
                        rect.transformation.rotation, shape.transformation.rotation, 
                    rect.transformation.offset, shape.transformation.offset)) return true;


            } break;
        }

    return false;

}

    




bool collision(CMPUT350::CollisionObject& obj1, CMPUT350::CollisionObject& obj2, CMPUT350::Point2D& pnt, bool& lineInvolved){
                                                                                // the point where an intersection with a line occurs
                                                                                // the bool is to let us know if there was an intersection involving a line intersect or nto
                                                                                // important for when a line is INSIDE a circle/rectangle.
                                                                                // lineInvolved is passed through all functions in this file


    if (!(BoundingBoxCollision(obj1, obj2))) return false; // check the bounding boxes first.

    lineInvolved = false;
    for ( auto& obj1_shape : obj1.GetShapes()) {
        ShapeTransfer shape_trans(obj1.GetLocation(), obj1.GetRotation(), obj1_shape);
        lineInvolved = obj1_shape.t == CMPUT350::ShapeType::kLine; // we know a line is involved in this collision

        for (auto& obj2_shape : obj2.GetShapes()) {
            ShapeTransfer shape_trans_2(obj2.GetLocation(), obj2.GetRotation(), obj2_shape);
            if (obj2_shape.t == CMPUT350::ShapeType::kLine) lineInvolved = true;

        switch(shape_trans.shape.t) {
            case CMPUT350::ShapeType::kLine:
             if (line_collides(shape_trans, shape_trans_2, pnt,lineInvolved)) return true;
             break;
            case CMPUT350::ShapeType::kCircle: 
            if (circle_collides(shape_trans, shape_trans_2, pnt,lineInvolved)) return true;
            break;
            case CMPUT350::ShapeType::kRect: 
            if (rect_collides(shape_trans,shape_trans_2, pnt,lineInvolved)) return true;
            break;
        
    }
}


}

return false;
}





