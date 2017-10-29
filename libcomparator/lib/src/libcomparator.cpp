#include "libcomparator.hpp"

double const prealocate = 0.01;

	DataProcess::DataProcess(){};

	void DataProcess::concatenate_HV(std::vector<IndexTransition>& data)
	{
		std::list<IndexTransition> listData(data.begin(), data.end());
		listData.sort([](const IndexTransition& a, const IndexTransition& b){return a.index < b.index;});
		std::transform(listData.begin(), --listData.end(), ++listData.begin(), ++listData.begin(),[](IndexTransition& a, IndexTransition& b)
		{
			if(a.index == b.index)
			{
				return IndexTransition{a.index, static_cast<Transition>(a.transition | b.transition)};
			}
			else
			{
				return a;
			}
		} );
		if( (*listData.begin()).index == (*(++listData.begin())).index)
		{
			 (*(listData.begin())).transition |=  (*(++listData.begin())).transition;
		}

		// for_each(listData.begin(), listData.end(), [](auto a){
		// 	static IndexTransition last = a;
		// 	if(last.index == a.index)
		// 	{
		// 		a.transition |= last.transition;
		// 	}

		// });

		for(auto it = listData.begin(); it != listData.end();)
		{
			if((*it).index == (*(++it)).index)
				it = listData.erase(--it);
		}

		data.clear();
		std::copy(listData.begin(), listData.end(), back_inserter(data));
	}


	template <class TYPE> IterateProcess<TYPE>::IterateProcess(cv::Mat_<TYPE> img, double lightThreshold, double colorThreshold, double colorBalance[]) : classifier(lightThreshold, colorThreshold, colorBalance)//should get type from img?
	{
		this->img = img;
	}

	template <class TYPE> std::vector<IndexTransition> IterateProcess<TYPE>::iterate_H()
	{
		std::vector<IndexTransition> result;
		result.reserve(sizeof(IndexTransition) * img.total() * prealocate);

		for(uint i = 0; i < img.rows; i++)		
		{
			uint rowIndex = i * img.step;
			for(uint j = 0; j < img.cols - channels; j += channels)
			{
				classifier.copy_pix(img.data + rowIndex + j, img.data + rowIndex + j + channels);
				switch (classifier.f_classifier())
				{
					case no: continue; break;
					case fwd: result.push_back(IndexTransition{rowIndex + j + channels, lToR}); break;
					case back: result.push_back(IndexTransition{rowIndex + j, rToL}); break;
				}
			}
		}
		return result;
	}

	template <class TYPE> std::vector<IndexTransition> IterateProcess<TYPE>::iterate_V()
	{
		std::vector<IndexTransition> result;
		result.reserve(sizeof(IndexTransition) * img.total() * prealocate);

		for(uint col = 0; col < img.cols - channels; col += channels)		
		{
			for(uint row = 0; row < img.rows - 1; row++)
			{
				classifier.copy_pix(img.data + row * img.step + col, img.data + ((row + 1) * img.step) + col);
				switch (classifier.f_classifier())
				{
					case no: continue; break;
					case fwd: result.push_back(IndexTransition{((row + 1) * img.step) + col, upToDw}); break;
					case back: result.push_back(IndexTransition{row * img.step + col, dwToUp}); break;
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
		if(brighter())
		{
			std::swap(pix0, pix1);//pix0 is now the dimmer
			result = fwd;
		}

		correct_pix0();
		if(light_distance() > lightThreshold)
			if(color_distance() < colorThreshold)
				return result;

		return (result = no);
	}

	template<class TYPE> void Classifier<TYPE>::correct_pix0()
	{
		pix0[0] = pix0[0] * colorBalance[0] + 0.5; pix0[1] = pix0[1] * colorBalance[1] + 0.5; pix0[2] = pix0[2] * colorBalance[2] + 0.5;//overflow ??
	}

	template <class TYPE> DTYPE Classifier<TYPE>::light_distance()//pix0 have to < pix1 and be corrected
	{
		lightDistance = (pix1[0] + pix1[1] + pix1[2]) - (pix0[0] + pix0[1] + pix0[2]);
		return lightDistance;
	}

	template <class TYPE> DTYPE Classifier<TYPE>::color_distance()
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
		specifyIT.iterate_H();
		specifyIT.iterate_V();
		specifyIT.iterate_HV();
	}