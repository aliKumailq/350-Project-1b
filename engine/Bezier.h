#pragma once
#include <vector>
#include "MathUtil.h"

class Bezier {
public:
	Bezier(const std::vector<CMPUT350::Point2D> &pts);
	
	inline CMPUT350::Point2D GetPoint(const float& t) const;
	inline CMPUT350::Point2D GetSlope(const float& t) const;
	
	static inline CMPUT350::Point2D GetPoint(const std::vector<CMPUT350::Point2D> &pts, const float& t);
	static inline CMPUT350::Point2D GetSlope(const std::vector<CMPUT350::Point2D> &pts, const float& t);	

    private:
    const std::vector<CMPUT350::Point2D> m_pts;
};
