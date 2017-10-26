#include "comparatorlib.hpp"
int const channels = 3;//only for convinience
double const prealocate = 0.01;


	template <class pTYPE>
	std::vector<int> iterate_H(cv::Mat img, void (*classifier)(pTYPE, pTYPE, double, double, cv::Point3_), double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
		std::vector<int> result;
		result.reserve(sizeof(int) * img.total() * prealocate);

		for(int i = 0; i < img.rows; i++)		
		{
			int rowIndex = i * img.step;
			for(int j = 0; j < img.cols - channels; j += channels)
			{
				switch (classifier(img[rowIndex + j], img[rowIndex + j + channels], lightThreshold, colorThreshold, colorBalance))
				{
					case no: continue;
					case backward: result.push_back(rowIndex + j);
					case forward: result.push_back(rowIndex + j + channels);
				}
			}
		}

		return result;
	}
	
	template <class pTYPE>
	std::vector<int> iterate_V(cv::Mat img, void (*classifier)(pTYPE, pTYPE, double, double, cv::Point3_), double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
		std::vector<int> result;
		result.reserve(sizeof(int) * img.total() * prealocate)

		for(int col = 0; col < img.cols - channels; col += channels)		
		{
			for(int row = 0; row < img.rows - 1; row++)
			{
				switch (classifier(img[row * img.step + col], img[((row + 1) * img.step) + col], lightThreshold, colorThreshold, colorBalance))
				{
					case no: continue;
					case forward: result.push_back(((row + 1) * img.step) + col);
					case backward: result.push_back(row * img.step + col);
				}
			}
		}
		return result;
	}

	template <class pTYPE>
	std::vector<int> iterate_HV(cv::Mat img, void (*classifier)(pTYPE, pTYPE, double, double, cv::Point3_), double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
		auto shadowH = iterate_H(img, classifier, lightThreshold, colorThreshold, colorBalance);
		auto shadowV = iterate_V(img, classifier, lightThreshold, colorThreshold, colorBalance);
		shadowH.insert(shadowH.end(), shadowV.begin(), shadowV.end());
		auto shadowHV = shadowH;
		return shadowHV;
	}

	template <class pTYPE>
	Transition color_light_classifier(pTYPE pix0in, pTYPE pix1in, double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
		pTYPE pix0 = new (pTYPE*)[channels]; std::memcpy(pix0, pix0in, channels * sizeof(pTYPE*));//no impact at source image
		pTYPE pix1 = new (pTYPE*)[channels]; std::memcpy(pix1, pix1in, channels * sizeof(pTYPE*));//no impact at source image

		Transition result = forward;
		if(brighter(pix0, pix1))
		{
			std::swap(pix0, pix1);//pix0 is now the dimmer
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
