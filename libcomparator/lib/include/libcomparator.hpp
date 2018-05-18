#ifndef COMPARATOR_HPP
	#define COMPARATOR_HPP
	#include <cstring>
	#include <opencv2/core/core.hpp>
	#include "math.h"
	#include <algorithm>
	#include <list>
	#include <vector>
	#include <utility>

	#ifdef WITH_TESTS
		#ifdef TEST_PRIVATE_PART
			#define private public
		#endif
	#endif

	typedef char unsigned UCHAR;
	

	template <class TYPE>
	class Classifier;


	template <class TYPE>
	class IterateProcess
	{
	public:
		IterateProcess();
		double iterate_H(TYPE* pix0, TYPE* pix1);
	private:		
		Classifier<TYPE> classifier;
	};

	
	template <class TYPE>
	class Classifier
	{
	public:
		Classifier();
		double color_distance(TYPE* pix0, TYPE* pix1);
	private:
	};
#endif