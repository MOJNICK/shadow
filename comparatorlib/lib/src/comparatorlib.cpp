#include "comparatorlib.hpp"
int const channels = 3;//only for convinience
double const prealocate = 0.01;

	template <class TYPE>
	IterateProcess<TYPE>::IterateProcess(cv::Mat_<TYPE> img)//should get type from img?
	{
		this.img = img;
		this.classifier = Classifier<TYPE*>::color_light_classifier;
	}


	template <class TYPE>
	std::vector<IndexTransition> IterateProcess<TYPE*>::iterate_H()
	{
		auto& img = this.img;
		auto& classifier = this.classifier;
		auto lightThreshold = this.lightThreshold;
		auto colorThreshold = this.colorThreshold;
		auto colorBalance = this.colorBalance;

		std::vector<IndexTransition> result;
		result.reserve(sizeof(IndexTransition) * img.total() * prealocate);

		for(int i = 0; i < img.rows; i++)		
		{
			int rowIndex = i * img.step;
			for(int j = 0; j < img.cols - channels; j += channels)
			{
				switch (classifier(img.data + rowIndex + j, img.data + rowIndex + j + channels, lightThreshold, colorThreshold, colorBalance))
				{
					case no: continue;
					case forward: result.push_back(IndexTransition{rowIndex + j + channels, leftToRight});
					case backward: result.push_back(IndexTransition{rowIndex + j, rightToLeft});
				}
			}
		}
		return result;
	}

	template <class TYPE>
	std::vector<IndexTransition> IterateProcess<TYPE*>::iterate_V()
	{
		auto& img = this.img;
		auto& classifier = this.classifier;
		auto lightThreshold = this.lightThreshold;
		auto colorThreshold = this.colorThreshold;
		auto colorBalance = this.colorBalance;

		std::vector<IndexTransition> result;
		result.reserve(sizeof(IndexTransition) * img.total() * prealocate)

		for(int col = 0; col < img.cols - channels; col += channels)		
		{
			for(int row = 0; row < img.rows - 1; row++)
			{
				switch (classifier(img.data + row * img.step + col, img.data + ((row + 1) * img.step) + col], lightThreshold, colorThreshold, colorBalance))
				{
					case no: continue;
					case forward: result.push_back(IndexTransition{((row + 1) * img.step) + col, upToDown});
					case backward: result.push_back(IndexTransition{row * img.step + col, downToUp});
				}
			}
		}
		return result;
	}

	template <class TYPE>
	std::vector<IndexTransition> IterateProcess<TYPE*>::iterate_HV()
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

	template <class TYPE>
	Transition Classifier<TYPE*>::color_light_classifier(TYPE* pix0, TYPE* pix1, double lightThreshold, double colorThreshold, cv::Point3_<double> colorBalance = cv::Point3_(1, 1, 1))
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

	template <class TYPE>
	int Classifier<TYPE*>::balanced_light_distance(cv::Point3_<double> colorBalance, TYPE* pix0, TYPE* pix1)//pix0 have to < pix1
	{
		return ((pix1[0] + pix1[1] + pix1[2]) - (pix0[0] * colorBalance[0] + pix0[1] * colorBalance[1] + pix0[2] * colorBalance[2]));
	}

	template <class TYPE*>
	int Classifier<TYPE*>::balanced_color_distance(cv::Point3_<double> colorBalance, TYPE* pix0, TYPE* pix1)
	{
		return pow(pix0[0] * colorBalance[0] - pix1[0], 2) + pow(pix0[1] * colorBalance[1] - pix1[1], 2) + pow(pix0[2] * colorBalance[2] - pix1[2], 2);
	}

	template<class TYPE>
	bool Classifier<TYPE*>::brighter(TYPE* pix0, TYPE* pix1)
	{
		return ((pix0[0] + pix0[1] + pix0[2]) > (pix1[0] + pix1[1] + pix1[2]) ? (true) : (false));
	}

	template<class TYPE>
	void Classifier<TYPE*>::correct_balance(cv::Point3_<double> colorBalance, TYPE* pix)//unused
	{
		pix[0] *= colorBalance[0]; pix[1] *= colorBalance[1]; pix[2] *= colorBalance[2];
	}
