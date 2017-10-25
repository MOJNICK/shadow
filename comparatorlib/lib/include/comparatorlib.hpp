#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP
#include "types.hpp"
#include "mat.hpp"
#include "math.h"
	template <class pTYPE>
	void iterate_H(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_));
	
	template <class pTYPE>
	void iterate_V(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_));

	template <class pTYPE>
	void iterate_HV(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_));

	template <class pTYPE>
	bool color_light_classifier(pTYPE pix0, pTYPE pix1, double lightThreshold, double colorThreshold, cv::Point3_ colorBallance);

	template <class pTYPE>
	int color_distance(pTYPE pix0, pTYPE pix1);

	template <class pTYPE>
	swap_pointer(pTYPE*, pTYPE*)

	template<class pType>
	bool brighter(pTYPE, pTYPE)



#endif