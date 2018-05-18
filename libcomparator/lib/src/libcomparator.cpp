#include "libcomparator.hpp"

double const prealocate = 0.01;//vector reserve


	template <class TYPE> IterateProcess<TYPE>::IterateProcess(cv::Mat& img, TYPE acceptanceLevel, double lightThreshold, double colorThreshold, double colorBalance[]) : classifier(acceptanceLevel, lightThreshold, colorThreshold, colorBalance)
	{
		if(! img.isContinuous() )
		{
			img = cv::Mat( cv::Size(200,100), CV_8UC(channels), cv::Scalar(0));
			cv::putText(img, "not coninuous", cv::Point(0,0), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,250), 1, 8, false);
			this->img = img;
		}
		else { this->img = img; }
	}

	template <class TYPE> std::vector<IndexTransition> IterateProcess<TYPE>::iterate_H()
	{
		std::vector<IndexTransition> result;

		for( int row = 0; row < img.rows; row++)		
		{
			int rowIndex = row * img.step;
			classifier.copy_pix(img.data + rowIndex, img.data + rowIndex + channels);
		}
		return result;
	}

	template <class TYPE> Classifier<TYPE>::Classifier(TYPE acceptanceLevel_, double lightThreshold_, double colorThreshold_, double colorBalance_[]) : lightThreshold{lightThreshold_ + 1}, colorThreshold{colorThreshold_}
	{
		if(acceptanceLevel_ != 0){ acceptanceLevel = acceptanceLevel_; }
		else{ acceptanceLevel = 1; }
		memcpy(colorBalance, colorBalance_, sizeof(double) * channels);	
	}

	template <class TYPE> void Classifier<TYPE>::copy_pix(TYPE pix0_[], TYPE pix1_[])
	{
		memcpy(pix0, pix0_, sizeof(TYPE) * channels);
		memcpy(pix1, pix1_, sizeof(TYPE) * channels);	
	}

	template <class TYPE> Transition Classifier<TYPE>::f_classifier()
	{
		Transition result = back;
		if(color_distance() < colorThreshold)
				return result;

		return (Transition::no);
	}
	
	template <class TYPE> double Classifier<TYPE>::color_distance()
	{
		double var = -(pix1[0] / static_cast<double>(pix0[0]) + pix1[1] / static_cast<double>(pix0[1]) + pix1[2] / static_cast<double>(pix0[2])) / channels;//for minimize color_distance
		return pow(pix1[0] / static_cast<double>(pix0[0]) + var, 2) + pow(pix1[1] / static_cast<double>(pix0[1]) + var, 2) + pow(pix1[2] / static_cast<double>(pix0[2]) + var, 2);
	}



	void instantiate()
	{
		double x[]={1.2,1.0,1.0};
		TYPE pix [] = {10,10,10};
		Classifier<TYPE> specifyCL(1, 1.0, 1.0, x);
		specifyCL.copy_pix(pix, pix);
		specifyCL.f_classifier();
		cv::Mat_<TYPE> mat(0,0);
		IterateProcess<TYPE> specifyIT( mat, 1, 1.0, 1.0, x );
	#ifdef WITH_TESTS
		specifyCL.set_parameters(1, 1.0, 1.0, x);
		#ifdef TEST_PRIVATE_PART
			specifyIT.iterate_H();
			specifyIT.iterate_V();
		#endif
	#endif
	}