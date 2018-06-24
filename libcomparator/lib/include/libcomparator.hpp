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
	#ifdef MASK_PROCESS
	class IterateProcessMask
	#else
	class IterateProcess
	#endif
	{
		static constexpr double prealocate = 0.01;//vector reserve
	public:
		#ifdef MASK_PROCESS
		IterateProcessMask
		#else
		IterateProcess
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
			assert(img.isContinuous());
			this->img = img;
			#ifdef MASK_PROCESS
			assert(mask.isContinuous());
			assert( img.rows == mask.rows && img.cols == mask.cols );
			this->mask = mask;
			#endif
		}

		static void concatenate_HV(std::vector<IndexTransition>& data)
		{
			if(data.size() < 2)
				return;

			std::stable_sort(data.begin(), data.end(), []( const IndexTransition& a, const IndexTransition& b ){ return a.col < b.col; });
			std::stable_sort(data.begin(), data.end(), []( const IndexTransition& a, const IndexTransition& b ){ return a.row < b.row; });

			uint validIdx = 0;
			for (uint idx = 1; idx < data.size(); idx++)
			{
				if( data[ validIdx ].same_position( data[ idx ] ) )
				{
					data[ validIdx ].transition |= data[ idx ].transition;			
				}
				else
				{
					std::swap( data[ ++validIdx ], data[ idx ] );
				}

			}
			data.resize(++validIdx);
		}

		std::vector<IndexTransition> iterate_HV()
		{
			std::vector<IndexTransition> detectedH = iterate_H();
			std::vector<IndexTransition> detectedV = iterate_V();
			detectedH.insert(detectedH.end(), detectedV.begin(), detectedV.end());
			std::vector<IndexTransition> detectedHV = detectedH;
			concatenate_HV( detectedHV );
			return detectedHV;
		}
	private:		
		cv::Mat_<TYPE> img;
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
		#ifdef MASK_PROCESS
			template class IterateProcessMask<TYPE>;
		#else
			template class IterateProcess<TYPE>;
		#endif
	#endif
#endif