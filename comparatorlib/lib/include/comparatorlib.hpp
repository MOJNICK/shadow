#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP
#include <cstring>
#include <opencv2/core/core.hpp>
//#include "types.hpp"
//#include "mat.hpp"
#include "math.h"
#include <vector>

int const channels = 3;//not a parameter, only for convinience

typedef char TYPE;

enum Transition
{
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
	
	template <class TYPE>
	class Classifier;

	template <class TYPE>
	class IterateProcess
	{
	public:
		IterateProcess(cv::Mat_<TYPE>, double, double, double[]);
		std::vector<IndexTransition> iterate_HV();
	private:		
		cv::Mat_<TYPE> img;//reference by default
		Classifier<TYPE> classifier;
		double lightThreshold;
		double colorThreshold;

		std::vector<IndexTransition> iterate_H();
		std::vector<IndexTransition> iterate_V();
		
	};
	
	template <class TYPE>
	class Classifier
	{
	public:
		Classifier(double, double, double[]);
		void copy(TYPE[], TYPE[]);
		Transition fClassifier();
	private:
		TYPE pix0[channels];
		TYPE pix1[channels];
		double lightThreshold;
		double colorThreshold;
		double colorBalance[channels];

		int balanced_light_distance();
		int balanced_color_distance();
		bool brighter();
		void correct_balance_pix0();
		void swap();
	};

#endif