#include "dataprocess.hpp"

DataProcess::DataProcess(){};

void DataProcess::concatenate_HV(std::vector<IndexTransition>& data)
{
	std::list<IndexTransition> listData(data.begin(), data.end());
	listData.sort([](const IndexTransition& a, const IndexTransition& b){return a.index < b.index;});
	std::transform(listData.begin(), --listData.end(), ++listData.begin(), ++listData.begin(),[](IndexTransition& a, IndexTransition& b)
	{
		if(a.index == b.index)
		{
			return IndexTransition{a.index, static_cast<Transition>(a.transition | b.transition)};
		}
		else
		{
			return b;
		}
	} );
	if( (*listData.begin()).index == (*(++listData.begin())).index)
	{
		 (*(listData.begin())).transition |=  (*(++listData.begin())).transition;
	}

	for(auto it = listData.begin(); it != listData.end();)
	{
		if((*it).index == (*(++it)).index)
			it = listData.erase(--it);
	}

	data.clear();
	std::copy(listData.begin(), listData.end(), back_inserter(data));
}