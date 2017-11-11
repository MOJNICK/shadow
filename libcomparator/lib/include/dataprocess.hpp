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
		ColorBalance( cv::Mat const &, uint ){};
		void balance( std::vector< const IndexTransition > & const );
		~ColorBalance(){};
	private:
		cv::Mat const & img;
		uint distance;
		TYPE acceptanceLevel;
		double balance[ channels ];
		uint weight;
		bool is_valid( IndexTransition& );
		void element_balance( IndexTransition& );
	}
#endif