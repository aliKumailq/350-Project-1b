#include "CollisionDetection.h"
#include "CollisionObject.h"
#include "RectHelpers.h"


namespace {
    using namespace CMPUT350;
    struct Transformation {

        Transformation(const Point2D& pnt, const float& r) : offset(pnt), rotation(r), cos(cosf(rotation)), sin(sinf(rotation)) {}

        Point2D transform(const Point2D& p_, bool reverse = false) const  {
            auto p = p_ - (reverse ? offset : 0);
            const float x = reverse ? (p.x * cos + p.y * sin) : (p.x * cos - p.y * sin);
            const float y = reverse ? (p.y * cos - p.x * sin) : (p.x * sin + p.y * cos);
            return Point2D(x, y) + (reverse ? 0 : offset);
        }

        CMPUT350::Point2D offset;
        float rotation;
        float cos;
        float sin;

    };

    struct ShapeTransfer{
        
        ShapeTransfer(const Point2D& pnt, const float& r, const Shape& shape) : transformation(pnt, r), shape(shape) {}

        Transformation transformation;
        Shape shape;

    };

    inline void ShapeTransferCircle(ShapeTransfer& circle) {
        assert(circle.shape.t == CMPUT350::ShapeType::kCircle);
        circle.shape.shape.circle.center = circle.transformation.transform(circle.shape.shape.circle.center);
    }

    inline void ShapeTransferLine(ShapeTransfer& line) {
        assert(line.shape.t == CMPUT350::ShapeType::kLine);
        line.shape.shape.line.p1 = line.transformation.transform(line.shape.shape.line.p1);
        line.shape.shape.line.p2 = line.transformation.transform(line.shape.shape.line.p2);
        return;
    }
}


static inline bool line_circle_collision(const ShapeTransfer& line_, const ShapeTransfer& circle_, CMPUT350::Point2D& pnt) {
    auto line__ = line_;
    auto circle__ = circle_;

    ShapeTransferCircle(circle__);
    ShapeTransferLine(line__);


    auto& line = line__.shape.shape.line;
    auto& circle = circle__.shape.shape.circle;




    auto closest_pnt = line.ClosestPoint(circle.center);
    auto distance_sqrd = closest_pnt.DistanceSquared(circle.center);

    if (distance_sqrd > SquareFloat(circle.radius)) return false;
    // if the square of the distance between closest point and radius is larger than the radius squared, then we DO NOT have a collision


    auto q_sqr = SquareFloat(circle.radius) - distance_sqrd;

    auto distance_p1_p = line.p1.Distance(closest_pnt);

    auto dir = (line.p2 - line.p1) * (1.0f / line.p1.Distance(line.p2));
    auto of = sqrt(q_sqr) * dir;




    pnt = closest_pnt - of;

    pnt = line__.transformation.transform(pnt, true);

    return true;


}


static inline bool line_rect_collision(const ShapeTransfer& line_, const ShapeTransfer& Rect ,CMPUT350::Point2D& pnt) {
    assert(Rect.shape.t == CMPUT350::ShapeType::kRect);

    auto line__ = line_;
    ShapeTransferLine(line__);
    auto& line = line__.shape.shape.line;

    auto verticies = RectHelpers::RectangleVerticies(Rect.shape.shape.rect);
    RectHelpers::TransformVerticies(verticies, Rect.transformation.offset, Rect.transformation.rotation);

    auto edges = RectHelpers::GetRectEdges(verticies);
    if (line.Crosses(edges.top,pnt)) return true;
    if (line.Crosses(edges.bottom, pnt)) return true;
    if (line.Crosses(edges.left, pnt)) return true;
    if (line.Crosses(edges.right, pnt)) return true;

    return false;


}

static inline bool line_collides(const ShapeTransfer& shape1, const ShapeTransfer& shape2, CMPUT350::Point2D& pnt) {
    assert(shape1.shape.t == CMPUT350::ShapeType::kLine);




        switch(shape2.shape.t) {
            case CMPUT350::ShapeType::kLine: {
                auto line1 = shape1;
                ShapeTransferLine(line1);

                auto line2 = shape2;
                ShapeTransferLine(line2);


                if (line1.shape.shape.line.Crosses(line2.shape.shape.line, pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                if(line_circle_collision(shape1,shape2, pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                if(line_rect_collision(shape1, shape2,pnt)) return true;


            } break;

        }
    
    return false;

}

static inline bool circle_rect_collision(const ShapeTransfer& circle, const ShapeTransfer& rect ,CMPUT350::Point2D& pnt) {
    assert(rect.shape.t == CMPUT350::ShapeType::kRect);

    auto verticies = RectHelpers::RectangleVerticies(rect.shape.shape.rect);
    auto edges = RectHelpers::GetRectEdges(verticies);

    auto top = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.top};
    auto bottom = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.bottom};
    auto right = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.right};
    auto left = ShapeTransfer{rect.transformation.offset, rect.transformation.rotation, edges.left};

    
    if (line_circle_collision(top,circle,pnt)) return true;
    if (line_circle_collision(bottom,circle, pnt)) return true;
    if (line_circle_collision(left, circle,pnt)) return true;
    if (line_circle_collision(right, circle,pnt)) return true;
     
    return false;


}






static inline bool circle_collides( const ShapeTransfer&circle, const ShapeTransfer& shape, CMPUT350::Point2D& pnt) {


        switch(shape.shape.t) {

            case CMPUT350::ShapeType::kLine: {

                if(line_circle_collision(shape, circle, pnt)) return true;;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                auto circle2 = shape; // punch me in the face
                auto circle1 = circle;
                ShapeTransferCircle(circle1);
                ShapeTransferCircle(circle2);

                if (!(circle1.shape.shape.circle.center.DistanceSquared(circle2.shape.shape.circle.center) > SquareFloat(circle.shape.shape.circle.radius + circle2.shape.shape.circle.radius))) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                if(circle_rect_collision(circle, shape, pnt)) return true;


            } break;
        }
    
    
    return false;



}


static inline bool rect_collides(const ShapeTransfer&rect, const ShapeTransfer& shape , CMPUT350::Point2D& pnt) {
    

        switch(shape.shape.t) {

            case CMPUT350::ShapeType::kLine: {

                if (line_rect_collision(shape, rect,pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kCircle: {
                if(circle_rect_collision(shape, rect ,pnt)) return true;

            } break;

            case CMPUT350::ShapeType::kRect: {

                if (RectangleCollision(rect.shape.shape.rect, shape.shape.shape.rect, 
                        rect.transformation.rotation, shape.transformation.rotation, 
                    rect.transformation.offset, shape.transformation.offset)) return true;


            } break;
        }

    return false;

}

    




bool collision(CMPUT350::CollisionObject& obj1, CMPUT350::CollisionObject& obj2, CMPUT350::Point2D& pnt) {

    //if (!(BoundingBoxCollision(obj1, obj2))) return false;



    for ( auto& obj1_shape : obj1.GetShapes()) {
        ShapeTransfer shape_trans(obj1.GetLocation(), obj1.GetRotation(), obj1_shape);

        for (auto& obj2_shape : obj2.GetShapes()) {
            ShapeTransfer shape_trans_2(obj2.GetLocation(), obj2.GetRotation(), obj2_shape);

        switch(shape_trans.shape.t) {
            case CMPUT350::ShapeType::kLine:
             if (line_collides(shape_trans, shape_trans_2, pnt)) return true;
             break;
            case CMPUT350::ShapeType::kCircle: 
            if (circle_collides(shape_trans, shape_trans_2, pnt)) return true;
            break;
            case CMPUT350::ShapeType::kRect: 
            if (rect_collides(shape_trans,shape_trans_2, pnt)) return true;
            break;
        
    }
}


}

return false;
}





