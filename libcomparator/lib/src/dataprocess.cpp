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

ColorBalance::ColorBalance( cv::Mat const & img, TYPE acceptanceLevel_, uint distance = 1 ):
img( img ), distance{ distance }, colorBalance{ 0, 0 , 0 }, weight{ 0 }
{
	acceptanceLevel = std::max( acceptanceLevel_, static_cast< TYPE >( 1 ) );
}

void ColorBalance::balance( std::vector< IndexTransition >& position )
{
	std::for_each( position.begin(), position.end(), [ this ]( IndexTransition& el )
	{
		element_balance( el );
	});

	double normalize = 0.0;
	std::for_each( colorBalance, colorBalance + channels, [ this, &normalize ]( double& el )
	{
		normalize += el;
	});
	normalize /= channels;
	std::for_each( colorBalance, colorBalance + channels, [ this, &normalize ]( double& el )
	{
		el /= weight * normalize;
	});
}

void ColorBalance::element_balance( IndexTransition const & shadow )
{
	Transition const & shtransition = shadow.transition;
	if( !is_valid( shadow.transition ) )
		return;

	uint brightRow = shadow.row;
	uint brightCol = shadow.col;
	
	if( shtransition & ( Transition::upToDw ) ){ brightRow -= distance; }
	if( shtransition & ( Transition::lToR ) ){ brightCol -= distance * channels; }
	if( shtransition & ( Transition::dwToUp ) ){ brightRow += distance; }
	if( shtransition & ( Transition::rToL ) ){ brightCol += distance * channels; }

	if( ( 0 <= brightRow && brightRow < img.rows ) && ( 0 <= brightCol && brightCol < img.cols ))
		{
			for( uint i = 0; i < channels; ++i )
			{
				bool rejectShadow = img.data[ shadow.row * img.step + shadow.col + i ] < acceptanceLevel;
				bool rejectBright = img.data[ brightRow * img.step + brightCol + i ] < acceptanceLevel;
				if( rejectShadow || rejectBright )
				{
					return;
				}
			}
			for( uint i = 0; i < channels; ++i )
			{
				colorBalance[ i ] += static_cast< double >( img.data[ brightRow * img.step + brightCol + i ] ) / img.data[ shadow.row * img.step + shadow.col + i ];
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

#ifdef WITH_TESTS
	double* ColorBalance::getColorBalance()
	{
		return colorBalance;
	}

	void ColorBalance::clear_balance()
	{
		for(auto el = colorBalance; el != colorBalance + channels; ++el)
		{
			*el = 0;
		}
	}
#endif