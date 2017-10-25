#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP
#include <cstring>
#include "types.hpp"
#include "mat.hpp"
#include "math.h"
#include <vector>
enum Transition
	no,
	forward,
	backward
};

	template <class pTYPE>
	void iterate_H(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_));
	
	template <class pTYPE>
	void iterate_V(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_));

	template <class pTYPE>
	void iterate_HV(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_));

	template <class pTYPE>
	Transition color_light_classifier(pTYPE, pTYPE, double lightThreshold, double colorThreshold, cv::Point3_ colorBalance);

	template <class pTYPE>
	int balanced_light_distance(pTYPE, pTYPE);

	template <class pTYPE>
	int balanced_color_distance(pTYPE, pTYPE);

	template <class pTYPE>
	void swap_pointer(pTYPE*, pTYPE*)

	template<class pTYPE>
	bool brighter(pTYPE, pTYPE)

	template<class pTYPE>
	void correct_balance(cv::Point3_, pTYPE)

#endif