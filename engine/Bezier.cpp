#include "Bezier.h"
#include <cassert>
#include "MathUtil.h"


namespace  {


    template<int I> // Note: I is the DEGREE of the bezier curve (i.e. I = 1 --> Linear, I = 2 --> Quadratic, etc)
    inline CMPUT350::Point2D GetBezierPoint(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        static_assert(I > 0);
        return (1-t) * GetBezierPoint<I - 1>(pts, starting_point_index, t) + t * GetBezierPoint<I - 1>(pts, starting_point_index + 1, t);
    }

    template<> // base case for t at linear bezier curve
    inline CMPUT350::Point2D GetBezierPoint<1>(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        const CMPUT350::Point2D& A = pts[starting_point_index];
        const CMPUT350::Point2D& B = pts[starting_point_index + 1];

        return ((1 - t) * A) + (t*B);
    }


    // The derivative, by the product rule, recursing the same way as GetBezierPoint
    template<int I>
    inline CMPUT350::Point2D GetBezierPoint_d(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        static_assert(I > 0);


        const auto lhs =  (-1)*GetBezierPoint<I - 1>(pts, starting_point_index, t) + (1-t) * GetBezierPoint_d<I - 1>(pts, starting_point_index, t);
        const auto rhs = GetBezierPoint<I - 1>(pts, starting_point_index + 1, t) + t * GetBezierPoint_d<I - 1>(pts, starting_point_index + 1, t);
        return lhs + rhs; // for the love of god let the compiler just write these two out.
    }

    template<> // Base Case for derivative of linear bezier curve
    inline CMPUT350::Point2D GetBezierPoint_d<1>(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
        const CMPUT350::Point2D& A = pts[starting_point_index];
        const CMPUT350::Point2D& B = pts[starting_point_index + 1];
        return B- A; 
    }


    constexpr size_t BEZIER_DIMENSION = 3;


}


// Store the control points; they have to cover at least one curve
Bezier::Bezier(const std::vector<CMPUT350::Point2D> &pts) : m_pts(pts) {
    assert(pts.size() > BEZIER_DIMENSION);
}

// The spline evaluated at t, by picking the curve t falls on and its position within that curve
CMPUT350::Point2D Bezier::GetPoint(const std::vector<CMPUT350::Point2D> &pts, const float& t) {
    assert(pts.size() > BEZIER_DIMENSION); 
    const int max_t = int((pts.size() - 1) / BEZIER_DIMENSION);
    assert(t >= 0 && t <= max_t);

    // t walks the chain: floor(t) picks the curve, the fraction says how far along it. At t == max_t
    // that index points one curve past the end, whose control points do not exist, so pin it to the
    // end of the last real curve instead -- which is the spline's final point.
    size_t line_segment = size_t(t);
    float local_t = t - float(int(t));
    if (line_segment >= size_t(max_t)) {
        line_segment = size_t(max_t - 1);
        local_t = 1.0f;
    }

    const size_t line_segment_starting_index = line_segment * BEZIER_DIMENSION;
    return GetBezierPoint<BEZIER_DIMENSION>(pts, line_segment_starting_index, local_t);

}

// The tangent at t, walking the chain the same way GetPoint does
CMPUT350::Point2D Bezier::GetSlope(const std::vector<CMPUT350::Point2D> &pts, const float& t) {
    assert(pts.size() > BEZIER_DIMENSION); 
    const int max_t = int((pts.size() - 1) / BEZIER_DIMENSION);
    assert(t >= 0 && t <= max_t);

    // Same walk as GetPoint, with the same clamp at the far end so the final slope does not read past
    // the last curve either.
    size_t line_segment = size_t(t);
    float local_t = t - float(int(t));
    if (line_segment >= size_t(max_t)) {
        line_segment = size_t(max_t - 1);
        local_t = 1.0f;
    }

    const size_t line_segment_starting_index = line_segment * BEZIER_DIMENSION;
    return GetBezierPoint_d<BEZIER_DIMENSION>(pts, line_segment_starting_index, local_t);
    

}


// The stored points, so a caller does not have to pass the list again
CMPUT350::Point2D Bezier::GetPoint(const float& t) const {
    return Bezier::GetPoint(m_pts, t);
}

// The stored points, same as above
CMPUT350::Point2D Bezier::GetSlope(const float & t) const {
    return Bezier::GetSlope(m_pts, t);
}
