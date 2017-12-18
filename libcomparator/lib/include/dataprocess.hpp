#ifndef DATAPOCESS_HPP
	#define DATAPOCESS_HPP
	#include "libcomparator.hpp"


	enum SideToClear
	{
		head = 0x01,
		tail = 0x10,
		both = head | tail
	};


	struct ColorStruct
	{
		friend DataProcess;

		double color[ channels ];

		ColorStruct();
		ColorStruct( double value );
		ColorStruct( std::initializer_list< double > l );
		ColorStruct& operator+=( ColorStruct const & src );
		ColorStruct& operator/=( double const divisor );
		ColorStruct& operator=( std::initializer_list< double > l );
		static double add_saturation( ColorStruct & first, ColorStruct & second );
		static double subtract_saturation( ColorStruct & first, ColorStruct & second );
		static bool less_saturation( ColorStruct & first, ColorStruct & second );
		static bool higher_saturation( ColorStruct & first, ColorStruct & second );
		static double add_HUE( ColorStruct & first, ColorStruct & second );
		static double subtract_HUE( ColorStruct & first, ColorStruct & second );
		static bool less_HUE( ColorStruct & first, ColorStruct & second );
		static bool higher_HUE( ColorStruct & first, ColorStruct & second );
	private:
		static double baseLevel; //temporary workaround ..?
		double saturation();
		static double saturation_cast( ColorStruct const & cs );
		double HUE();
		static double HUE_cast( ColorStruct const & cs );
	};


	class ColorBalance
	{
	public:
		friend DataProcess;
		ColorBalance( cv::Mat const &, TYPE, uint );
		void balance( std::vector< IndexTransition >& );
		~ColorBalance(){};
		#ifdef WITH_TESTS
			ColorStruct getColorBalance( uint idx = 0 );
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


	class DataProcess
	{
	public:
		DataProcess();
		static void concatenate_HV( std::vector< IndexTransition >& );
		static double hue_base_level( std::vector< ColorStruct > colorBalance );
		/*template< class TYPE, class Compare >
			static void 
			outliner( std::vector<TYPE> & dataset, double diffMult = 1, SideToClear side = both, Compare fun = []( TYPE& a, TYPE& b ){ return a < b; });*/

		template< class TypeIn, class TYPE, class Compare, class BaseArithm, class Cast >
			static void
			outliner( std::vector<TYPE> & dataset, double diffMult = 1, SideToClear side = both,
						Compare less = []( TYPE& a, TYPE& b ){ return a < b; },
						Compare higher = []( TYPE& a, TYPE& b ){ return a > b; },
						BaseArithm add = []( TYPE& a, TYPE& b ){ return a + b; },
						BaseArithm subtract = []( TYPE& a, TYPE& b ){ return a - b; },
						Cast cast_arithm_arg = [](TYPE& a)->TypeIn{ return static_cast<TypeIn>(a); }
					);
	};
#endif