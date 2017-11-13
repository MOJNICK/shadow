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

ColorStruct::ColorStruct()
{
	std::fill( color, color + channels, 0);
}

ColorStruct::ColorStruct( std::initializer_list< double > l )
{
	std::transform( l.begin(), l.end(), color, []( double ld ){ return ld; } );
}

ColorStruct& ColorStruct::operator+=( ColorStruct const & src )
{
	for( int i = 0; i < channels; ++i )
	{
		color[ i ] += src.color[ i ];
	}
	return *this;
}

ColorStruct& ColorStruct::operator/=( double const divisor )
{
	for( int i = 0; i < channels; ++i )
	{
		color[ i ] /= divisor;
	}
	return *this;
}

ColorStruct& ColorStruct::operator=( std::initializer_list< double > l )
{
	std::transform( l.begin(), l.end(), color, []( double ld ){ return ld; } );
	return *this;
}

bool ColorStruct::compare_saturation( ColorStruct const & first, ColorStruct const & second )
{
	return true;
}

bool ColorStruct::compare_HUE( ColorStruct const & first, ColorStruct const & second )
{
	return true;
}

ColorBalance::ColorBalance( cv::Mat const & img, TYPE acceptanceLevel_, uint distance = 1 ):
img( img ), distance{ distance } //, colorBalance{ ColorStruct{ 0, 0 , 0 } }
{
	acceptanceLevel = std::max( acceptanceLevel_, static_cast< TYPE >( 1 ) );
}

void ColorBalance::balance( std::vector< IndexTransition >& positions )
{
	std::for_each( positions.begin(), positions.end(), [ this ]( IndexTransition & el )
	{
		element_balance( el );
	});
	
	// DataProcess::outliner( colorBalance, 1 );
	
	ColorStruct sumBalance;

	std::for_each( colorBalance.begin(), colorBalance.end(), [ &sumBalance ]( ColorStruct const & el )
	{
		sumBalance += el;
	} );

	double normalizer = ( sumBalance.color[0] + sumBalance.color[1] + sumBalance.color[2] ) / channels;

	sumBalance /= colorBalance.size() * normalizer;
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
			ColorStruct _colorBalance { .0, .0, .0 };
			for( uint i = 0; i < channels; ++i )
			{
				_colorBalance.color[i] = static_cast< double >( img.data[ brightRow * img.step + brightCol + i ] ) / img.data[ shadow.row * img.step + shadow.col + i ];
			}
			colorBalance.push_back( _colorBalance );
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

template< class TYPE, class Compare >
void
DataProcess::outliner( std::vector<TYPE> & dataset, double diffMult, SideToClear side, Compare fun )
{
    TYPE median( 0 );
    TYPE Q1( 0 );
    TYPE Q3( 0 );
    TYPE diff( 0 );
    TYPE downLim( 0 );
    TYPE upLim( 0 );
    double d_2_0 = 2.0;
    std::sort( dataset.begin(), dataset.end(), fun );

    if( ( dataset.size() % 2 ) == 0 )
        median = ( dataset )[ dataset.size() / 2 ];
    else
        median = ( ( dataset )[ static_cast< uint >( dataset.size() / d_2_0 ) ] + ( dataset )[ static_cast< uint >( dataset.size() / d_2_0 - 1.0 + 0.5) ] ) / d_2_0;

    if( ( dataset.size() % 4 ) == 0 )
        Q1 = ( dataset )[ dataset.size() / 4 ];
    else
        Q1 = ( ( dataset )[ static_cast< uint >( dataset.size() / 4.0 + 0.5 ) ] + ( dataset )[ static_cast< uint >( dataset.size() / 4.0 - 1 + 0.5 ) ] ) / d_2_0;

    if( ( ( dataset.size() * 3 ) % 4 ) == 0 )
        Q3 = ( dataset )[ dataset.size() * 3 / 4 ];
    else
        Q3 = ( ( dataset )[ static_cast< uint >( dataset.size() * 3.0/4.0 + 0.5 ) ] + ( dataset )[ static_cast< uint >( dataset.size() * 3.0/4.0 - 1 + 0.5 ) ] ) / d_2_0;

    diff = Q3-Q1;
    diff *= diffMult;
    downLim = Q1 - diff;
    upLim = Q3 + diff;

    //higher upLim;
    uint begg = 0;
    for( begg = dataset.size() - 1; ; begg--) //dataset.size -1 == dataser.end
    {
        if( dataset[ begg ] > upLim )
            continue;
        else
            break;
    }

    uint endd = 0;
    for(endd = 0; ; endd++ )
    {
        if( dataset[ endd ] < downLim )
            continue;
        else
            break;
    }
    
    if( side & SideToClear{ tail } )
    	dataset.resize( begg + 1 );//rm tail

    if( side & SideToClear{ head } )
    	dataset.erase( dataset.begin(), dataset.begin() + endd );//rm head
}

#ifdef WITH_TESTS
	ColorStruct ColorBalance::getColorBalance()
	{
		if( colorBalance.size() )
		{
			return colorBalance[0];
		}
		else
		{
			return ColorStruct{ .0, .0, .0 };
		}
	}

	void ColorBalance::clear_balance()
	{
		colorBalance.resize(0);
	}
#endif