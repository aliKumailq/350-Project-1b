#ifndef BEZIER_H
#define BEZIER_H

#include <vector>
#include "MathUtil.h"

// A piecewise cubic Bezier spline. The control points arrive as one flat list in groups of four,
// where each group shares its first point with the previous group's last, so n curves are described
// by 3n+1 points. The parameter t walks the whole chain: [0,1] is the first curve, [1,2] the
// second, through to t == (points-1)/3 at the far end of the last one.
class Bezier {
public:
	Bezier(const std::vector<CMPUT350::Point2D> &pts);
	
	CMPUT350::Point2D GetPoint(const float& t) const;
	CMPUT350::Point2D GetSlope(const float& t) const;
	
	static CMPUT350::Point2D GetPoint(const std::vector<CMPUT350::Point2D> &pts, const float& t);
	static CMPUT350::Point2D GetSlope(const std::vector<CMPUT350::Point2D> &pts, const float& t);	

	private:
	const std::vector<CMPUT350::Point2D> m_pts;
};

#endif  // BEZIER_H
