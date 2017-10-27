#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP
#include <cstring>
#include "types.hpp"
#include "mat.hpp"
#include "math.h"
#include <vector>

typedef char TYPE;

enum Transition
	no,
	forward,
	backward,
	upToDown,
	downToUp,
	leftToRight,
	rightToLeft
};

struct IndexTransition
{
	int index;
	Transition transition;
};

	class Classifier<TYPE>;

	template <class TYPE>
	class IterateProcess
	{
	public:
		IterateProcess(Mat<TYPE> img);
		std::vector<IndexTransition> iterate_HV();
	
	private:		
		Mat<TYPE> img;//reference by default
		double lightThreshold;
		double colorThreshold;
		cv::Point3_ colorBalance;
		Transition (*classifier)(TYPE*, TYPE*, double, double, cv::Point3_);

		std::vector<IndexTransition> iterate_H();
		std::vector<IndexTransition> iterate_V();
		
	}
	
	template <class TYPE>
	class Classifier
	{
	public:
		static Transition color_light_classifier(TYPE*, TYPE*, double lightThreshold, double colorThreshold, cv::Point3_ colorBalance);
	private:
		static int balanced_light_distance(TYPE*, TYPE*);
		static int color_distance(TYPE*, TYPE*);
		static bool brighter(TYPE*, TYPE*);
		static void correct_balance(cv::Point3_, TYPE*)
	}

#endif