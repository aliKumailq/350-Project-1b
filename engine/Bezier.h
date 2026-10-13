#include <vector>
#include "MathUtil.h"

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
