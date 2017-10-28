#include "comparatorlib.hpp"

double const prealocate = 0.01;

	template <class TYPE> IterateProcess<TYPE>::IterateProcess(cv::Mat_<TYPE> img, double lightThreshold, double colorThreshold, double colorBalance[]) : classifier(lightThreshold, colorThreshold, colorBalance)//should get type from img?
	{
		this->img = img;
	}

	template <class TYPE> std::vector<IndexTransition> IterateProcess<TYPE>::iterate_H()
	{
		std::vector<IndexTransition> result;
		result.reserve(sizeof(IndexTransition) * img.total() * prealocate);

		for(int i = 0; i < img.rows; i++)		
		{
			int rowIndex = i * img.step;
			for(int j = 0; j < img.cols - channels; j += channels)
			{
				classifier.copy_pix(img.data + rowIndex + j, img.data + rowIndex + j + channels);
				switch (classifier.f_classifier())
				{
					case no: continue;
					case forward: result.push_back(IndexTransition{rowIndex + j + channels, leftToRight});
					case backward: result.push_back(IndexTransition{rowIndex + j, rightToLeft});
				}
			}
		}
		return result;
	}

	template <class TYPE> std::vector<IndexTransition> IterateProcess<TYPE>::iterate_V()
	{
		std::vector<IndexTransition> result;
		result.reserve(sizeof(IndexTransition) * img.total() * prealocate);

		for(int col = 0; col < img.cols - channels; col += channels)		
		{
			for(int row = 0; row < img.rows - 1; row++)
			{
				classifier.copy_pix(img.data + row * img.step + col, img.data + ((row + 1) * img.step) + col);
				switch (classifier.f_classifier())
				{
					case no: continue;
					case forward: result.push_back(IndexTransition{((row + 1) * img.step) + col, upToDown});
					case backward: result.push_back(IndexTransition{row * img.step + col, downToUp});
				}
			}
		}
		return result;
	}

	template <class TYPE> std::vector<IndexTransition> IterateProcess<TYPE>::iterate_HV()
	{
		std::vector<IndexTransition> detectedH = iterate_H();
		std::vector<IndexTransition> detectedV = iterate_V();
		detectedH.insert(detectedH.end(), detectedV.begin(), detectedV.end());
		std::vector<IndexTransition> detectedHV = detectedH;
		return detectedHV;
	}







	template <class TYPE> Classifier<TYPE>::Classifier(double lightThreshold_, double colorThreshold_, double colorBalance_[]) : lightThreshold{lightThreshold_}, colorThreshold{colorThreshold_}
	{
		memcpy(colorBalance, colorBalance_, sizeof(TYPE) * channels);	
	}

	template <class TYPE> void Classifier<TYPE>::copy_pix(TYPE pix0_[], TYPE pix1_[])
	{
		memcpy(pix0, pix0_, sizeof(TYPE) * channels);
		memcpy(pix1, pix1_, sizeof(TYPE) * channels);	
	}

	template <class TYPE> Transition Classifier<TYPE>::f_classifier()
	{
		Transition result = forward;
		if(brighter())
		{
			std::swap(pix0, pix1);//pix0 is now the dimmer
			result = backward;
		}

		correct_pix0();
		if(light_distance() > lightThreshold)
			if(color_distance() < colorThreshold)
				return result;

		return (result = no);
	}

	template<class TYPE> void Classifier<TYPE>::correct_pix0()
	{
		pix0[0] *= colorBalance[0]; pix0[1] *= colorBalance[1]; pix0[2] *= colorBalance[2];//overflow ??
	}

	template <class TYPE> int Classifier<TYPE>::light_distance()//pix0 have to < pix1 and be corrected
	{
		lightDistance = ((pix1[0] + pix1[1] + pix1[2]) - (pix0[0] + pix0[1] + pix0[2] ));
		return lightDistance;
	}

	template <class TYPE> int Classifier<TYPE>::color_distance()
	{
		DTYPE var = -(pix1[0] - pix0[0] + pix1[1] - pix0[1] + pix1[2] - pix0[2]) / channels;//for minimize color_distance
		return pow(pix1[0] - pix0[0] + var, 2) + pow(pix1[1] - pix0[1] + var, 2) + pow(pix1[2] - pix0[2] + var, 2);
	}

	template<class TYPE> bool Classifier<TYPE>::brighter()
	{
		return ((pix0[0] + pix0[1] + pix0[2]) > (pix1[0] + pix1[1] + pix1[2]) ? (true) : (false));
	}

	template<class TYPE> void Classifier<TYPE>::swap()//unused
	{

	}

	void instantiate()
	{
		double x[]={1.2,1.0,1.0};
		TYPE pix [] = {10,10,10};
		Classifier<TYPE> specifyCL(1.0,1.0,x);
		specifyCL.copy_pix(pix, pix);
		specifyCL.f_classifier();
		IterateProcess<TYPE> specifyIT(cv::Mat_<TYPE>(0,0),1.0,1.0,x);
	}