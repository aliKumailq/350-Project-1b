#include "Bezier.h"
#include <cassert>
#include "MathUtil.h"


namespace  {


    template<int I> // Note: I is the DEGREE of the bezier curve (i.e. I = 1 --> Linear, I = 2 --> Quadratic, etc)
    inline CMPUT350::Point2D GetBezierPoint(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        static_assert(I > 0);
        return (1-t) * GetBezierPoint<I - 1>(pts, starting_point_index, t) + t * GetBezierPoint<I - 1>(pts, starting_point_index + 1, t);
    }

    template<>
    inline CMPUT350::Point2D GetBezierPoint<1>(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        const CMPUT350::Point2D& A = pts[starting_point_index];
        const CMPUT350::Point2D& B = pts[starting_point_index + 1];

        return ((1 - t) * A) + (t*B);
    }


    template<int I>
    inline CMPUT350::Point2D GetBezierPoint_d(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        static_assert(I > 0);


        const auto lhs =  (-1)*GetBezierPoint<I - 1>(pts, starting_point_index, t) + (1-t) * GetBezierPoint_d<I - 1>(pts, starting_point_index, t);
        const auto rhs = GetBezierPoint<I - 1>(pts, starting_point_index + 1, t) + t * GetBezierPoint_d<I - 1>(pts, starting_point_index + 1, t);
        return lhs + rhs; // for the love of god let the compiler just write these two out.
    }

    template<>
    inline CMPUT350::Point2D GetBezierPoint_d<1>(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        const CMPUT350::Point2D& A = pts[starting_point_index];
        const CMPUT350::Point2D& B = pts[starting_point_index + 1];
        return B- A; 
    }


    constexpr size_t BEZIER_DIMENSION = 3;


}


Bezier::Bezier(const std::vector<CMPUT350::Point2D> &pts) : m_pts(pts) {
    assert(pts.size() > BEZIER_DIMENSION);
}

inline CMPUT350::Point2D Bezier::GetPoint(const std::vector<CMPUT350::Point2D> &pts, const float& t) {
    assert(pts.size() > BEZIER_DIMENSION); 
    const int max_t = int((pts.size() - 1) / BEZIER_DIMENSION);
    assert(t >= 0 && t <= max_t);

    const size_t line_segment = t;
    const size_t line_segment_starting_index = line_segment * BEZIER_DIMENSION;
    return GetBezierPoint<BEZIER_DIMENSION>(pts, line_segment_starting_index, t - int(t));

}

inline CMPUT350::Point2D Bezier::GetSlope(const std::vector<CMPUT350::Point2D> &pts, const float& t) {
    assert(pts.size() > BEZIER_DIMENSION); 
    const int max_t = int((pts.size() - 1) / BEZIER_DIMENSION);
    assert(t >= 0 && t <= max_t);

    const size_t line_segment = t;
    const size_t line_segment_starting_index = line_segment * BEZIER_DIMENSION;
    return GetBezierPoint_d<BEZIER_DIMENSION>(pts, line_segment_starting_index, t - int(t));
    

}


CMPUT350::Point2D Bezier::GetPoint(const float& t) const {
    return Bezier::GetPoint(m_pts, t);
}

CMPUT350::Point2D Bezier::GetSlope(const float & t) const {
    return Bezier::GetSlope(m_pts, t);
}