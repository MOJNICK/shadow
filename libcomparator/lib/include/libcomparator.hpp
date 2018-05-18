#ifndef COMPARATOR_HPP
	#define COMPARATOR_HPP
	#include <cmath>

	typedef char unsigned UCHAR;
	
	template <class TYPE>
	class IterateProcess
	{
	public:
		IterateProcess();
		double iterate_H(TYPE* pix0, TYPE* pix1);
		double color_distance(TYPE* pix0, TYPE* pix1);
	private:		
	};

#endif