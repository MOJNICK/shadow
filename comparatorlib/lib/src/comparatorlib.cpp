#include "comparatorlib.hpp"
int const channels = 3;//only for convinience
double const prealocate = 0.01;

	template <class pTYPE>
	IterateProcess<pTYPE>::IterateProcess(Mat img)//should get type from img
	{
		this.img = img;
		this.classifier = Classifier<pTYPE>::color_light_classifier;
	}


	template <class pTYPE>
	std::vector<int> IterateProcess<pTYPE>::iterate_H()
	{
		auto& img = this.img;
		auto& classifier = this.classifier;
		auto lightThreshold = this.lightThreshold;
		auto colorThreshold = this.colorThreshold;
		auto colorBalance = this.colorBalance;

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
	std::vector<int> IterateProcess<pTYPE>::iterate_V()
	{
		auto& img = this.img;
		auto& classifier = this.classifier;
		auto lightThreshold = this.lightThreshold;
		auto colorThreshold = this.colorThreshold;
		auto colorBalance = this.colorBalance;

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
	std::vector<int> IterateProcess<pTYPE>::iterate_HV()
	{
		auto& img = this.img;
		auto& classifier = this.classifier;
		auto lightThreshold = this.lightThreshold;
		auto colorThreshold = this.colorThreshold;
		auto colorBalance = this.colorBalance;

		auto detectedH = iterate_H(img, classifier, lightThreshold, colorThreshold, colorBalance);
		auto detectedV = iterate_V(img, classifier, lightThreshold, colorThreshold, colorBalance);
		detectedH.insert(detectedH.end(), detectedV.begin(), detectedV.end());
		auto detectedHV = std::move(detectedH);
		return detectedHV;
	}

	template <class pTYPE>
	Transition Classifier<pType>::color_light_classifier(pTYPE pix0, pTYPE pix1, double lightThreshold, double colorThreshold, cv::Point3_ colorBalance = cv::Point3_(1, 1, 1))
	{
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
	int Classifier<pType>::balanced_light_distance(cv::Point3_ colorBalance, pTYPE pix0, pTYPE pix1)//pix0 have to < pix1
	{
		return ((pix1[0] + pix1[1] + pix1[2]) - (pix0[0] * colorBalance[0] + pix0[1] * colorBalance[1] + pix0[2] * colorBalance[2]));
	}

	template Classifier<pType>::<class pTYPE>
	int Classifier<pType>::balanced_color_distance(cv::Point3_ colorBalance, pTYPE pix0, pTYPE pix1)
	{
		return pow(pix0[0] * colorBalance[0] - pix1[0], 2) + pow(pix0[1] * colorBalance[1] - pix1[1], 2) + pow(pix0[2] * colorBalance[2] - pix1[2], 2);
	}

	template<class pTYPE>
	bool Classifier<pType>::brighter(pTYPE pix0, pTYPE pix1)
	{
		return ((pix0[0] + pix0[1] + pix0[2]) > (pix1[0] + pix1[1] + pix1[2]) ? (true) : (false));
	}

	template<class pTYPE>
	void Classifier<pType>::correct_balance(cv::Point3_ colorBalance, pTYPE pix)//unused
	{
		pix[0] *= colorBalance[0]; pix[1] *= colorBalance[1]; pix[2] *= colorBalance[2];
	}
