#include "Bezier.h"
#include <cassert>
#include "MathUtil.h"

static constexpr size_t BEZIER_DIMENSION = 3;

Bezier::Bezier(const std::vector<CMPUT350::Point2D> &pts) : m_pts(pts) {
    assert(pts.size() > BEZIER_DIMENSION);
}

static inline CMPUT350::Point2D GetLinearBezierPoint(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
    const CMPUT350::Point2D& A = pts[starting_point_index];
    const CMPUT350::Point2D& B = pts[starting_point_index + 1];

    return (t*A) + ((1 - t) * B);
}

static inline CMPUT350::Point2D GetQuadraticBezierPoint(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
    return t * GetLinearBezierPoint(pts, starting_point_index, t) + (1-t) * GetLinearBezierPoint(pts, starting_point_index + 1, t);
}

static inline CMPUT350::Point2D GetCubicBezierPoint(const std::vector<CMPUT350::Point2D> &pts, const size_t& starting_point_index,const float& t) {
    return t * GetQuadraticBezierPoint(pts, starting_point_index, t) + (1-t) * GetQuadraticBezierPoint(pts, starting_point_index + 1, t);
}

CMPUT350::Point2D Bezier::GetPoint(const std::vector<CMPUT350::Point2D> &pts, const float& t) {
    assert(pts.size() > BEZIER_DIMENSION); 
    const float max_t = (pts.size() - 1) / BEZIER_DIMENSION;
    assert(t >= 0 && t <= max_t);

    const size_t line_segment = t;
    const size_t line_segment_starting_index = line_segment * BEZIER_DIMENSION;
    return GetCubicBezierPoint(pts, line_segment_starting_index, t - int(t));

}


CMPUT350::Point2D Bezier::GetPoint(const float& t) const {
    return Bezier::GetPoint(m_pts, t);
}