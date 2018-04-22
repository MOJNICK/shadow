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

	enum Transition
	{
		empty = 0,
		no = 1 << 0,
		unknown = no << 1,

		fwd = no << 2,
		back = fwd << 1,
		biFwdBack = fwd | back,

		upToDw = no << 4,
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
			return ( ( row == b.row ) && ( col == b.col ) ) ? true : false;
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


	template <class TYPE>
	class IterateProcess
	{
	public:
		IterateProcess( cv::Mat&, TYPE, double, double, double[] );
		std::vector<IndexTransition> iterate_HV();
	private:		
		cv::Mat_<TYPE> img;//reference by default?
		Classifier<TYPE> classifier;

		std::vector<IndexTransition> iterate_H();
		std::vector<IndexTransition> iterate_V();
		
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
		void swap();
	};

	extern "C"
	{

	}

#endif