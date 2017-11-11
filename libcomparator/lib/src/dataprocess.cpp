#include "dataprocess.hpp"

DataProcess::DataProcess(){};

void DataProcess::concatenate_HV(std::vector<IndexTransition>& data)
{
	if(data.size() < 2)
		return;

	std::stable_sort(data.begin(), data.end(), []( const IndexTransition& a, const IndexTransition& b ){ return a.col < b.col; });
	std::stable_sort(data.begin(), data.end(), []( const IndexTransition& a, const IndexTransition& b ){ return a.row < b.row; });

	uint validIdx = 0;
	for (uint idx = 1; idx < data.size(); idx++)
	{
		if( data[ validIdx ].same_position( data[ idx ] ) )
		{
			data[ validIdx ].transition |= data[ idx ].transition;
			
		}
		else
		{
			std::swap( data[ ++validIdx ], data[ idx ] );
		}

	}
	data.resize(++validIdx);
}

ColorBalance::ColorBalance( cv::Mat const & img, TYPE acceptanceLevel, uint distance = 1 ):
img( img ), distance{ distance }, colorBalance{ 0, 0 ,0 }, weight{ 0 }
{
	std::min( acceptanceLevel, static_cast< TYPE >( 1 ) );
}

void ColorBalance::balance( std::vector< IndexTransition >& position )
{
	std::for_each( position.begin(), position.end(), [ this ]( IndexTransition& el )
	{
		element_balance( el );
	});
	std::for_each( colorBalance, colorBalance + channels, [ this ]( double& el )
	{
		el /= weight;
	});
}

void ColorBalance::element_balance( IndexTransition const & el )
{
	if( !is_valid( el.transition ) )
		return;

	uint shadowRow = el.row;
	uint shadowCol = el.col;
	
	Transition const & transition = el.transition;
	if( transition & ( Transition::upToDw ) ){ shadowRow += distance; }
	if( transition & ( Transition::lToR ) ){ shadowCol += distance; }
	if( transition & ( Transition::dwToUp ) ){ shadowRow -= distance; }
	if( transition & ( Transition::rToL ) ){ shadowCol -= distance; }

	if( ( 0 <= shadowRow && shadowRow < img.rows ) && ( 0 <= shadowCol && shadowCol < img.cols ))
		{
			for( uint i = 0; i < channels; ++i )
			{
				if( img.data[ shadowRow * img.step + shadowCol + i ] < acceptanceLevel )
				{
					return;
				}
				colorBalance[ i ] += img.data[ el.row * img.step + el.col + i ] / static_cast< double >( img.data[ shadowRow * img.step + shadowCol + i ] );
				++weight;
			}
		}
}

bool ColorBalance::is_valid( Transition const & transition )
{
	switch( transition )
	{
		case upToDw: break;
		case lToR: break;
		case dwToUp: break;
		case rToL: break;		
		case biLUp: break;
		case biLDw: break;
		case biRDw: break;
		case biRUp: break;
		default: return false;
	}
	return true;
}