#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP
#include <cstring>
#include "types.hpp"
#include "mat.hpp"
#include "math.h"
#include <vector>

typedef char TYPE;
typedef char* pTYPE;

enum Transition
	no,
	forward,
	backward
};
	class Classifier<pTYPE>;

	template <class pTYPE>
	class IterateProcess
	{
	public:
		IterateProcess(Mat img);
		std::vector<int> iterate_HV();
	
	private:		
		Mat img;//reference by default
		double lightThreshold;
		double colorThreshold;
		cv::Point3_ colorBalance;
		Transition (*classifier)(pTYPE, pTYPE, double, double, cv::Point3_);

		std::vector<int> iterate_H();
		std::vector<int> iterate_V();
		
	}
	
	template <class pTYPE>
	class Classifier
	{
	public:
		static Transition color_light_classifier(pTYPE, pTYPE, double lightThreshold, double colorThreshold, cv::Point3_ colorBalance);
	private:
		static int balanced_light_distance(pTYPE, pTYPE);
		static int color_distance(pTYPE, pTYPE);
		static bool brighter(pTYPE, pTYPE);
		static void correct_balance(cv::Point3_, pTYPE)
	}

#endif