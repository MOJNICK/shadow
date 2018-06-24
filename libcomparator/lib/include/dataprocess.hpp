#ifndef DATAPOCESS_HPP
	#define DATAPOCESS_HPP
	#include "libcomparator.hpp"
	#ifdef WITH_TESTS
		#include <iostream>
	#endif

	enum SideToClear
	{
		head = 0x01,
		tail = 0x10,
		both = head | tail
	};

	enum BalanceMode
	{
		color_balance = 0,
		brightness = 1
	};

	class DataProcess
	{
	public:
		DataProcess() = default;
		/*template< class TYPE, class Compare >
			static void 
			outliner( std::vector<TYPE> & dataset, double diffMult = 1, SideToClear side = both, Compare fun = []( TYPE& a, TYPE& b ){ return a < b; });*/

		template< class TypeIn, class TYPE, class Compare, class BaseArithm, class Cast >
			static void
			outliner( std::vector<TYPE> & dataset, double diffMult = 1, SideToClear side = both,
						Compare less = []( TYPE& a, TYPE& b ){ return a < b; },
						BaseArithm add = []( TYPE& a, TYPE& b ){ return a + b; },
						BaseArithm subtract = []( TYPE& a, TYPE& b ){ return a - b; },
						Cast cast_arithm_arg = [](TYPE& a)->TypeIn{ return static_cast<TypeIn>(a); }
					);
	};

	struct ColorStruct
	{
	public:
		friend DataProcess;

		ColorStruct();
		ColorStruct( double value );
		ColorStruct( std::initializer_list< double > l );

		operator double*(){ return color; }//
		double& operator[]( uint idx ){ return color[idx];}

		ColorStruct& operator+=( ColorStruct const & src );
		ColorStruct& operator*=( double const divisor );
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

		void set_baseLevel( double baseLevel_ ){ baseLevel = baseLevel_;}
		double get_baseLevel() const {return baseLevel;}

		void set_color(unsigned char index, double value){color[index] = value;}
		double get_color(unsigned char index) const {return color[index];}

		double saturation();
		double HUE();//0 for test only, ColorBalance::baseLevel
		static double saturation_cast( ColorStruct const & cs );
		static double HUE_cast( ColorStruct const & cs );

		double accumulate_color();
	private:
		double color[ channels ];
		double baseLevel;
	};


	class ColorBalance
	{
	public:
		friend DataProcess;
		
		ColorBalance( cv::Mat const &, TYPE, uint );
		ColorStruct balance( std::vector< IndexTransition > const & inputPositions, BalanceMode mode = color_balance );
		~ColorBalance(){};
		#ifdef WITH_TESTS
			ColorStruct getColorBalance( uint idx = 0 );
			void clear_balance();
		#endif
	private:
		ColorStruct inputPositionsBalance;//result
		cv::Mat const & img;
		uint distance;
		TYPE acceptanceLevel;
		double baseLevel;//should be pushed to thi->colorBalances[].baseLevel

		std::vector< ColorStruct > colorBalances;//temporary output balances of inputPositions	

		void push_element_balance( IndexTransition const & );
		double set_colorBalances_baseLevel();
		static bool is_valid( Transition const & );
	};
#endif