#ifndef DATAPOCESS_HPP
	#define DATAPOCESS_HPP
	#include "libcomparator.hpp"

	enum SideToClear
	{
		head = 0x01,
		tail = 0x10,
		both = head | tail
	};

	class DataProcess
	{
	public:
		DataProcess();
		static void concatenate_HV( std::vector< IndexTransition >& );
		template< class TYPE, class Compare > 
			static void 
			outliner( std::vector<TYPE> & dataset, double diffMult = 1, SideToClear side = both, Compare fun = []( TYPE& a, TYPE& b ){ return a < b; });
	};

	struct ColorStruct
	{
		double color[channels];

		ColorStruct();
		ColorStruct( std::initializer_list< double > l );
		ColorStruct& operator+=( ColorStruct const & src );
		ColorStruct& operator/=( double const divisor );
		ColorStruct& operator=( std::initializer_list< double > l );

		static bool compare_saturation( ColorStruct const & first, ColorStruct const & second );
		static bool compare_HUE( ColorStruct const & first, ColorStruct const & second );
	};

	class ColorBalance
	{
	public:
		ColorBalance( cv::Mat const &, TYPE, uint );
		void balance( std::vector< IndexTransition >& );
		template< class TYPE, class Compare >
			friend void
			DataProcess::outliner( std::vector<TYPE> & dataset, double diffMult, SideToClear side, Compare fun  );
		~ColorBalance(){};
		#ifdef WITH_TESTS
			ColorStruct getColorBalance();
			void clear_balance();
		#endif
	private:
		cv::Mat const & img;
		uint distance;
		TYPE acceptanceLevel;
		std::vector< ColorStruct > colorBalance;

		void element_balance( IndexTransition const & );
		static bool is_valid( Transition const & );
	};
#endif