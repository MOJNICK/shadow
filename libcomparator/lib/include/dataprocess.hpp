#ifndef DATAPOCESS_HPP
	#define DATAPOCESS_HPP
	#include "libcomparator.hpp"


	class DataProcess
	{
	public:
		DataProcess();
		static void concatenate_HV(std::vector<IndexTransition>&);
		static void vector_to_2Darray(std::vector<IndexTransition>&, int, int);
	};

#endif