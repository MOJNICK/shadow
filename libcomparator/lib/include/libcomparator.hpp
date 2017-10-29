#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP
#include <cstring>
#include <opencv2/core/core.hpp>
#include "math.h"
#include <algorithm>
#include <list>
#include <vector>
#include <utility>

#ifdef TEST_PRIVATE_PART
#define private public
#endif

typedef unsigned int uint;
int const channels = 3;//not a parameter, only for convinience
typedef unsigned char TYPE;
typedef int DTYPE;

enum Transition
{
	no = 0,
	fwd = 1 << 1,
	back = 1 << 2,
	biFwdBack= fwd | back,
	upToDw = 1 << 3,
	dwToUp = 1 << 4,
	biUpDw = upToDw | dwToUp,
	lToR = 1 << 5,
	rToL = 1 << 6,
	biLR = lToR | rToL,
	biLUp = lToR | upToDw,
	biLDw = lToR | dwToUp,
	biRUp = rToL | upToDw,
	biRDw = rToL | dwToUp,
	all = biLR | biUpDw,
};

inline Transition& operator|=(Transition& a, const Transition& b)
{return a = static_cast<Transition>((a) | (b));}

struct IndexTransition
{
	long unsigned int index;
	Transition transition;
};
	

	class DataProcess
	{
	public:
		DataProcess();
		static void concatenate_HV(std::vector<IndexTransition>&);
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

		std::vector<IndexTransition> iterate_H();
		std::vector<IndexTransition> iterate_V();
		
	};
	
	template <class TYPE>
	class Classifier
	{
	public:
		Classifier(double, double, double[]);
		void copy_pix(TYPE[], TYPE[]);
		Transition f_classifier();
	private:
		TYPE pix0[channels];
		TYPE pix1[channels];
		double lightThreshold;
		double colorThreshold;
		double colorBalance[channels];
		DTYPE lightDistance;

		void correct_pix0();
		DTYPE light_distance();
		DTYPE color_distance();
		bool brighter();
		void swap();
	};

#endif