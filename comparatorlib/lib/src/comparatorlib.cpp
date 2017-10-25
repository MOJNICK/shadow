#include "comparatorlib.hpp"
	template <class pTYPE>
	void iterate_H(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_))
	{

	}
	
	template <class pTYPE>
	void iterate_V(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_))
	{

	}

	template <class pTYPE>
	void iterate_HV(cv::Mat, void (*)(pTYPE, pTYPE, double, double, cv::Point3_))
	{

	}

	template <class pTYPE>
	bool color_light_classifier(pTYPE pix0, pTYPE pix1, double lightThreshold, double colorThreshold, cv::Point3_ colorBallance)
	{
		if(brighter(pix0, pix1))
			swap_pointer(&pix0, &pix1);
		if(light_distance(pix0, pix1) > lightThreshold)
			if(color_distance(pix0, pix1))

	}

	template <class pTYPE>
	int color_distance(pTYPE pix0, pTYPE pix1)
	{
		return pow(pix0[0] - pix1[0], 2) + pow(pix0[1] - pix1[1], 2) + pow(pix0[2] - pix1[2], 2);
	}

	template <class pTYPE>
	swap_pointer(pTYPE* p0, pTYPE* p1)
	{
		pTYPE* tmp = p0;
		*p0 = *p1;
		*p1 = *tmp;
	}

	template<class pType>
	bool brighter(pTYPE pix0, pTYPE pix1)
	{
		return (pix0[0] + pix0[1] + pix0[2]) > (pix1[0] + pix1[1] + pix1[2]) ? true : false;
	}
