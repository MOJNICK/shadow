#include "libcomparator.hpp"

template <class TYPE> IterateProcess<TYPE>::IterateProcess(){}

template <class TYPE> double IterateProcess<TYPE>::iterate_H(TYPE* pix0, TYPE* pix1)
{
	return color_distance(pix0, pix1);
}

template <class TYPE> double IterateProcess<TYPE>::color_distance(TYPE* pix0, TYPE* pix1)
{
	double var = -(pix1[0] / static_cast<double>(pix0[0]) + pix1[1] / static_cast<double>(pix0[1]) + pix1[2] / static_cast<double>(pix0[2]));//for minimize color_distance
	return std::pow(pix1[0] / static_cast<double>(pix0[0]) + var, 2) + std::pow(pix1[1] / static_cast<double>(pix0[1]) + var, 2) + std::pow(pix1[2] / static_cast<double>(pix0[2]) + var, 2);
}

void instantiate()
{
	UCHAR pix [] = {10,10,10};
	
	IterateProcess<UCHAR> specifyIT;
	specifyIT.iterate_H(pix, pix);
}