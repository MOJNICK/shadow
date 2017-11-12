#ifndef DATAPOCESS_HPP
	#define DATAPOCESS_HPP
	#include "libcomparator.hpp"

	class DataProcess
	{
	public:
		DataProcess();
		static void concatenate_HV( std::vector< IndexTransition >& );
	};

	struct ColorStruct
	{
		double color[channels];
	};

	class ColorBalance
	{
	public:
		ColorBalance( cv::Mat const &, TYPE, uint );
		void balance( std::vector< IndexTransition >& );
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
		static bool is_valid( Transition const & );
		void element_balance( IndexTransition const & );
	};
#endif