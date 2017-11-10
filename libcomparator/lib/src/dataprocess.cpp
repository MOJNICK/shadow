#include "dataprocess.hpp"

DataProcess::DataProcess(){};

void DataProcess::concatenate_HV(std::vector<IndexTransition>& data)
{
	std::list<IndexTransition> listData(data.begin(), data.end());
	listData.sort([](const IndexTransition& a, const IndexTransition& b){return a.row < b.row;});
	std::transform(listData.begin(), --listData.end(), ++listData.begin(), ++listData.begin(),[](IndexTransition& a, IndexTransition& b)
	{
		if( a.same_position( b ) )
		{
			return IndexTransition{a.row, a.col, static_cast<Transition>(a.transition | b.transition)};
		}
		else
		{
			return b;
		}
	} );
	if( (*listData.begin()).same_position(*(++listData.begin())) )
	{
		 (*(listData.begin())).transition |=  (*(++listData.begin())).transition;
	}

	for(auto it = listData.begin(); it != listData.end();)
	{
		if( (*it).same_position(*(++it)) )
			it = listData.erase(--it);
	}

	data.clear();
	std::copy(listData.begin(), listData.end(), back_inserter(data));
}