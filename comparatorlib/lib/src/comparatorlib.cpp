#include "comparatorlib.hpp"
const int channels = 3;//only for convinience
double prealocate = 0.01;

	template <class pTYPE>
	std::vector<cv::Point_<int>> iterate_H(cv::Mat img, void (*classifier)(pTYPE, pTYPE, double, double, cv::Point3_), double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
		std::vector<cv::Point_<int>> result;
		result.reserve(sizeof(cv::Point_<int>) * img.total() * prealocate)
		if(img.isContinuous())
		{
			for(int i = 0; i < img.rows; i++)		
				for(int j = i * img.cols; j < i * img.cols - channels; j += channels)
				{
					switch (classifier(img[j], img[j + channels], lightThreshold, colorThreshold, colorBalance))
					{
						// case no: continue;
						// case forward: result.push_back(cv::Point_<int>(i, j + channels));
						// case backward: result.push_back(cv::Point_<int>(i, j));
					}
				}
		}
		else
		{
			//,at
		}
		return result;
	}
	
	template <class pTYPE>
	std::vector<cv::Point_<int>> iterate_V(cv::Mat img, void (*classifier)(pTYPE, pTYPE, double, double, cv::Point3_), double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
		std::vector<cv::Point_<int>> result;
		result.reserve(sizeof(cv::Point_<int>) * img.total() * prealocate)
		if(img.isContinuous())
		{
			for(int j = 0; j < img.cols - channels; j += channels)		
				for(int i = j; i < j * img.rows - img.cols; i+=img.cols)
				{
					switch (classifier(img[i], img[i + img.cols], lightThreshold, colorThreshold, colorBalance))
					{
						// case no: continue;
						// case forward: result.push_back(cv::Point_<int>(i, j + channels));
						// case backward: result.push_back(cv::Point_<int>(i, j));
					}
				}
		}
		else
		{
			//,at
		}
		return result;
	}

	template <class pTYPE>
	void iterate_HV(cv::Mat img, void (*)(pTYPE, pTYPE, double, double, cv::Point3_))
	{

	}

	template <class pTYPE>
	vectorlPoint_<int> save_result()

	template <class pTYPE>
	Transition color_light_classifier(pTYPE pix0in, pTYPE pix1in, double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
		pTYPE pix0 = new (pTYPE*)[3]; std::memcpy(pix0, pix0in, 3 * sizeof(pTYPE*));//no impact at source image
		pTYPE pix1 = new (pTYPE*)[3]; std::memcpy(pix1, pix1in, 3 * sizeof(pTYPE*));//no impact at source image

		Transition result = forward;
		if(brighter(pix0, pix1))
		{
			swap_pointer(&pix0, &pix1);//pix0 is now the dimm
			result = backward;
		}

		if(balanced_light_distance(colorBalance, pix0, pix1) > lightThreshold)//ballance applied
			if(color_distance(colorBalance, pix0, pix1) < colorThreshold)
				return result;

		return (result = no);
	}

	template <class pTYPE>
	int balanced_light_distance(cv::Point3_ colorBalance, pTYPE pix0, pTYPE pix1)//pix0 have to < pix1
	{
		correct_balance(colorBalance, pix0);
		return ((pix1[0] + pix1[1] + pix1[2]) - (pix0[0] + pix0[1] + pix0[2]));
	}

	template <class pTYPE>
	int color_distance(cv::Point3_ colorBalance, pTYPE pix0, pTYPE pix1)
	{
		return pow(pix0[0] - pix1[0], 2) + pow(pix0[1] - pix1[1], 2) + pow(pix0[2] - pix1[2], 2);
	}

	template <class pTYPE>
	void swap_pointer(pTYPE* p0, pTYPE* p1)
	{
		pTYPE* tmp = p0;
		*p0 = *p1;
		*p1 = *tmp;
	}

	template<class pTYPE>
	bool brighter(pTYPE pix0, pTYPE pix1)
	{
		return ((pix0[0] + pix0[1] + pix0[2]) > (pix1[0] + pix1[1] + pix1[2]) ? (true) : (false));
	}

	template<class pTYPE>
	void correct_balance(cv::Point3_ colorBalance, pTYPE pix)
	{
		pix[0] *= colorBalance[0]; pix[1] *= colorBalance[1]; pix[2] *= colorBalance[2];
	}
