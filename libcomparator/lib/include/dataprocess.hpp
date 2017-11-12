#ifndef DATAPOCESS_HPP
	#define DATAPOCESS_HPP
	#include "libcomparator.hpp"

	class DataProcess
	{
	public:
		DataProcess();
		static void concatenate_HV( std::vector< IndexTransition >& );
	};

	class ColorBalance
	{
	public:
		ColorBalance( cv::Mat const &, TYPE, uint );
		void balance( std::vector< IndexTransition >& );
		~ColorBalance(){};
		#ifdef WITH_TESTS
			double* getColorBalance();
			void clear_balance();
		#endif
	private:
		cv::Mat const & img;
		uint distance;
		TYPE acceptanceLevel;
		double colorBalance[ channels ];
		uint weight;
		static bool is_valid( Transition const & );
		void element_balance( IndexTransition const & );
	};
#endif