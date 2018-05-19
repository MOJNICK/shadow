#ifndef COMPARATOR_HPP
	#define COMPARATOR_HPP
	#include <cstring>
	#include <opencv2/core/core.hpp>
	#include "math.h"
	#include <algorithm>
	#include <list>
	#include <vector>
	#include <utility>

	#ifdef WITH_TESTS
		#ifdef TEST_PRIVATE_PART
			#define private public
		#endif
	#endif

	#include <iostream>

	typedef unsigned int uint;
	unsigned char const channels = 3;//not a parameter, only for convinience
	typedef unsigned char TYPE;
	typedef int DTYPE;
	uint const distinctDir = 4;
	uint const shiftToDistinct = 4;
	uint const transDirCombi = 16;//4 distinct directions
	uint const directionBit = 4;

	enum Transition
	{
		empty = 0,
		no = 1 << 0,
		unknown = no << 1,

		fwd = no << 2,
		back = fwd << 1,
		biFwdBack = fwd | back,

		upToDw = no << directionBit,
		lToR = upToDw << 1,
		dwToUp = upToDw << 2,
		rToL = upToDw << 3,

		biLUp = lToR | upToDw,
		biLDw = biLUp << 1,
		biRDw = biLUp << 2,
		biRUp = upToDw | rToL,
		
		biUpDw = upToDw | dwToUp,
		biLR = biUpDw << 1,

		triUp = rToL | upToDw | lToR,
		triL = triUp << 1,
		triDw = triUp << 2,
		triR = triUp << 3,
					
		all = biLR | biUpDw

	};

	inline Transition operator|( Transition const & a, Transition const & b )
	{ return static_cast< Transition >( static_cast< int >( a ) | static_cast< int >( b ) ); }

	inline Transition& operator|=( Transition& a, Transition const & b )
	{ return a = a | b; }

	inline Transition operator<<( Transition const & a, uint const b )
	{ return static_cast< Transition >( static_cast< uint >( a ) << b ); }

	inline Transition& operator<<=( Transition& a, uint const b )
	{ return a = a << b; }

	class IndexTransition
	{
	public:
		int row;
		int col;
		Transition transition;

		//IndexTransition(IndexTransition const & indexTransition): row{indexTransition.row}, col{indexTransition.col}, transition{indexTransition.transition}{}
		bool operator ==( IndexTransition const &it) const
		{
			if( this->row == it.row && this->col == it.col && this->transition == it.transition )
			{
				return true;
			}
			{
				return false;
			}
		}

		bool same_position( IndexTransition& b ) const
		{
			return ( row == b.row ) && ( col == b.col );
		}

		uint index(cv::Mat& img)
		{
			return row * img.step + col * channels;
		}

		uint bw_index(cv::Mat& img)
		{
			return row * img.step + col;
		}

		uint conv_pix_to_subPix_col()
		{
			return col /= channels;
		}

		uint get_pix_to_subPix_col() const
		{
			return col / channels;
		}

		uint conv_subPix_to_pix_col()
		{
			return col *= channels;
		}
	};
	

	class DataProcess;

	template <class TYPE>
	class Classifier;


	template <class TYPE>//, bool with_mask = false>
	#ifndef MASK_PROCESS
	class IterateProcess
	#else
	class IterateProcessMask
	#endif
	{
		static constexpr double prealocate = 0.01;//vector reserve
	public:
		#ifndef MASK_PROCESS
		IterateProcess
		#else
		IterateProcessMask
		#endif
		(
			cv::Mat& img,
			TYPE     acceptanceLevel,
			double   lightThreshold,
			double   colorThreshold,
			double   colorBalance[],
			cv::Mat  mask = cv::Mat()
		)
		:
		classifier(acceptanceLevel, lightThreshold, colorThreshold, colorBalance)
		{
			if(! img.isContinuous() )
			{
				img = cv::Mat( cv::Size(200,100), CV_8UC(channels), cv::Scalar(0));
				cv::putText(img, "not coninuous", cv::Point(0,0), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,250), 1, 8, false);
				this->img = img;
			}
			else { this->img = img; }
		}

		std::vector<IndexTransition> iterate_HV()
		{
			std::vector<IndexTransition> detectedH = iterate_H();
			std::vector<IndexTransition> detectedV = iterate_V();
			detectedH.insert(detectedH.end(), detectedV.begin(), detectedV.end());
			std::vector<IndexTransition> detectedHV = detectedH;
			return detectedHV;
		}
	private:		
		cv::Mat_<TYPE> img;//reference by default?
		Classifier<TYPE> classifier;
	#ifdef MASK_PROCESS
		cv::Mat mask;//8UC1
		bool is_mask_ok(int row, int col){return mask.data[row*mask.cols + col/3] > 0;}
	#endif 

		std::vector<IndexTransition> iterate_H()
		{
			std::vector<IndexTransition> result;
			result.reserve(sizeof(IndexTransition) * img.total() * prealocate);

			for( int row = 0; row < img.rows; row++)		
			{
				int rowIndex = row * img.step;
				for(int col = 0; col < img.cols - channels; col += channels * sizeof(TYPE))
				{
					#ifdef MASK_PROCESS
					if(!is_mask_ok(row, col))
					{
						continue;
					}
					#endif
					classifier.copy_pix(img.data + rowIndex + col, img.data + rowIndex + col + channels);
					switch (classifier.f_classifier())
					{
						case no: continue; break;
						case fwd: result.push_back( IndexTransition{ row, col + channels, lToR } ); break;
						case back: result.push_back( IndexTransition{ row, col, rToL } ); break;
					}
				}
			}
			std::for_each( result.begin(), result.end(), [](auto& it){
				it.col /= channels;
			});
			return result;
		}
		std::vector<IndexTransition> iterate_V()
		{
			std::vector<IndexTransition> result;
			result.reserve(sizeof(IndexTransition) * img.total() * prealocate);

			for(int col = 0; col < img.cols - channels; col += channels * sizeof(TYPE))		
			{
				for(int row = 0; row < img.rows - 1; row++)
				{
					#ifdef MASK_PROCESS
					if(!is_mask_ok(row, col))
					{
						continue;
					}
					#endif
					classifier.copy_pix(img.data + row * img.step + col, img.data + ((row + 1) * img.step) + col);
					switch (classifier.f_classifier())
					{
						case no: continue; break;
						case fwd: result.push_back( IndexTransition{ row + 1, col, upToDw } ); break;
						case back: result.push_back( IndexTransition{ row, col, dwToUp } ); break;
					}
				}
			}
			std::for_each( result.begin(), result.end(), [](auto& it){
				it.col /= channels;
			});
			return result;
		}
	};

	
	template <class TYPE>
	class Classifier
	{
	public:
		Classifier( TYPE, double, double, double[] );
		void copy_pix(TYPE[], TYPE[]);
		Transition f_classifier();
		#ifdef WITH_TESTS
			void set_parameters( TYPE, double, double, double[] );
		#endif
	private:
		TYPE pix0[channels];
		TYPE pix1[channels];
		TYPE acceptanceLevel;
		double lightThreshold;
		double colorThreshold;
		double colorBalance[channels];
		double lightDistance;

		void correct_pix0();
		double color_distance();
		double light_distance();

		bool brighter();
	};
	
	#ifdef WITH_TESTS
		template class Classifier<TYPE>;
		#ifndef MASK_PROCESS
			template class IterateProcess<TYPE>;
		#else
			template class IterateProcessMask<TYPE>;
		#endif
	#endif

	template <class type, bool selector=true>
	class Selectable
	{
	public:
		Selectable(){std::cout<<"\n TRUE here \n";};
	};

	
	template<> 
	class Selectable<char, false>
	{
	public:
		Selectable(){std::cout<<"\n FALSE here \n";};	
	};
template class Selectable<char>;
template class Selectable<char,false>;
#endif