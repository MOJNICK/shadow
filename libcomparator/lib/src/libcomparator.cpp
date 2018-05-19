#include "libcomparator.hpp"
#include "dataprocess.hpp"


	template <class TYPE>
	Classifier<TYPE>::Classifier(TYPE acceptanceLevel_, double lightThreshold_, double colorThreshold_, double colorBalance_[]) : lightThreshold{lightThreshold_ + 1}, colorThreshold{colorThreshold_}
	{
		if(acceptanceLevel_ != 0){ acceptanceLevel = acceptanceLevel_; }
		else{ acceptanceLevel = 1; }
		memcpy(colorBalance, colorBalance_, sizeof(double) * channels);	
	}

	template <class TYPE>
	void Classifier<TYPE>::copy_pix(TYPE pix0_[], TYPE pix1_[])
	{
		memcpy(pix0, pix0_, sizeof(TYPE) * channels);
		memcpy(pix1, pix1_, sizeof(TYPE) * channels);	
	}

	template <class TYPE>
	Transition Classifier<TYPE>::f_classifier()
	{
		Transition result = back;
		if(brighter())// pix0 > pix1
		{
			std::swap(pix0, pix1);//pix0 is now the dimmer
			result = fwd;
		}
		if(pix1[0] < acceptanceLevel | pix1[1] < acceptanceLevel | pix1[2] < acceptanceLevel )
		{
			return (Transition::no);
		}

		correct_pix0();//correct balance
		if(light_distance() > lightThreshold)
			if(color_distance() < colorThreshold)
				return result;

		return (Transition::no);
	}
	
	#ifdef WITH_TESTS
		template <class TYPE>
		void Classifier<TYPE>::set_parameters( TYPE acceptanceLevel_, double lightThreshold_, double colorThreshold_, double colorBalance_[] )
		{
		 	if(acceptanceLevel_ != 0){ acceptanceLevel = acceptanceLevel_; }
		 	else{ acceptanceLevel = 1; }
			lightThreshold = lightThreshold_ + 1;
			colorThreshold = colorThreshold_;
			memcpy(colorBalance, colorBalance_, sizeof(double) * channels);	
		}
	#endif

	template<class TYPE>
	void Classifier<TYPE>::correct_pix0()
	{
		pix0[0] = pix0[0] * colorBalance[0] + 0.5; pix0[1] = pix0[1] * colorBalance[1] + 0.5; pix0[2] = pix0[2] * colorBalance[2] + 0.5;//overflow ??
	}

	template <class TYPE>
	double Classifier<TYPE>::light_distance()//pix0 have to < pix1 and be corrected
	{
		std::for_each(pix0, pix0 + channels, [](TYPE& el){if(el == 0){el = 1;}});// 0 -> 1 division
		lightDistance = static_cast<double>(pix1[0] + pix1[1] + pix1[2]) / static_cast<double>(pix0[0] + pix0[1] + pix0[2]);
		return lightDistance;
	}

	template <class TYPE>
	double Classifier<TYPE>::color_distance()
	{
		double var = -(pix1[0] / static_cast<double>(pix0[0]) + pix1[1] / static_cast<double>(pix0[1]) + pix1[2] / static_cast<double>(pix0[2])) / channels;//for minimize color_distance
		return pow(pix1[0] / static_cast<double>(pix0[0]) + var, 2) + pow(pix1[1] / static_cast<double>(pix0[1]) + var, 2) + pow(pix1[2] / static_cast<double>(pix0[2]) + var, 2);
	}

	template<class TYPE>
	bool Classifier<TYPE>::brighter()
	{
		return (pix0[0] + pix0[1] + pix0[2]) > (pix1[0] + pix1[1] + pix1[2]);
	}

	// void instantiate()
	// {
	// 	double x[]={1.2,1.0,1.0};
	// 	TYPE pix [] = {10,10,10};
	// 	Classifier<TYPE> specifyCL(1, 1.0, 1.0, x);
	// 	specifyCL.copy_pix(pix, pix);
	// 	specifyCL.f_classifier();
	// 	cv::Mat_<TYPE> mat(0,0);
	// 	IterateProcess<TYPE> specifyIT( mat, 1, 1.0, 1.0, x );
	// 	specifyIT.iterate_HV();
	// #ifdef WITH_TESTS
	// 	specifyCL.set_parameters(1, 1.0, 1.0, x);
	// 	#ifdef TEST_PRIVATE_PART
	// 		specifyIT.iterate_H();
	// 		specifyIT.iterate_V();
	// 	#endif
	// #endif
	// }