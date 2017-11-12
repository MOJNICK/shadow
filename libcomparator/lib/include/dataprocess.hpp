#ifndef DATAPOCESS_HPP
	#define DATAPOCESS_HPP
	#include "libcomparator.hpp"

	class DataProcess
	{
	public:
		DataProcess();
		static void concatenate_HV( std::vector< IndexTransition >& );
		template <class TYPE> static void outliner( std::vector<TYPE> & dataset, double diffMult = 1 );
	};

	struct ColorStruct
	{
		double color[channels];

		ColorStruct();
		ColorStruct( std::initializer_list< double > l );
		ColorStruct& operator+=( ColorStruct const & src );
		ColorStruct& operator/=( double const divisor );
		ColorStruct& operator=( std::initializer_list< double > l );
		bool operator<( ColorStruct const & first );
	};

	class ColorBalance
	{
	public:
		ColorBalance( cv::Mat const &, TYPE, uint );
		void balance( std::vector< IndexTransition >& );
		template <class TYPE> friend void DataProcess::outliner( std::vector<TYPE> & dataset, double diffMult );
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