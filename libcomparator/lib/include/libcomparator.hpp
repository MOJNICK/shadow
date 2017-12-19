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
	int const channels = 3;//not a parameter, only for convinience
	typedef unsigned char TYPE;
	typedef int DTYPE;

		enum Transition
		{
			no = 1 << 0,
			fwd = 1 << 1,
			back = 1 << 2,
			biFwdBack = fwd | back,

			upToDw = 1 << 3,
			lToR = upToDw << 1,
			dwToUp = upToDw << 2,
			rToL = upToDw << 3,

			biLUp = lToR | upToDw,
			biLDw = biLUp << 1,
			biRDw = biLUp << 2,
			biRUp = upToDw | rToL,
			
			biUpDw = upToDw | dwToUp,
			biLR = biUpDw << 1,
						
			all = biLR | biUpDw
		};

		inline Transition operator|( Transition const & a, Transition const & b )
		{ return static_cast< Transition >( static_cast< int >( a ) | static_cast< int >( b ) ); }

		inline Transition& operator|=( Transition& a, Transition const & b )
		{ return a = a | b; }


		class IndexTransition
		{
		public:
			unsigned int row;
			unsigned int col;
			Transition transition;

			IndexTransition(IndexTransition const & indexTransition): row{indexTransition.row}, col{indexTransition.col}, transition{indexTransition.transition}{}

			bool same_position( IndexTransition& b )
			{
				return ( ( row == b.row ) && ( col == b.col ) ) ? true : false;
			}

			unsigned int index(cv::Mat& img)
			{
				return row * img.step + col;
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