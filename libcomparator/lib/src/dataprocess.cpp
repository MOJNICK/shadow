#include "dataprocess.hpp"

double const d0upLim = 0.00001;//hue 0 limit

ColorStruct::ColorStruct()
{
	std::fill( color, color + channels, 0);
}

ColorStruct::ColorStruct( double value )
{
	std::fill( color, color + channels, value);
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

double ColorStruct::add_saturation( ColorStruct & first, ColorStruct & second )
{
	return first.saturation() + second.saturation();
}

double ColorStruct::subtract_saturation( ColorStruct & first, ColorStruct & second )
{
	return first.saturation() - second.saturation();
}

bool ColorStruct::less_saturation( ColorStruct & first, ColorStruct & second )
{
	return first.saturation() < second.saturation();
}

bool higher_saturation( ColorStruct & first, ColorStruct & second )
{
	return first.saturation() > second.saturation();
}

double ColorStruct::add_HUE( ColorStruct & first, ColorStruct & second )
{
	return first.HUE() + second.HUE();
}

double ColorStruct::subtract_HUE( ColorStruct & first, ColorStruct & second )
{
	return first.HUE() - second.HUE();
}

bool ColorStruct::less_HUE( ColorStruct & first, ColorStruct & second )
{
	return first.HUE() < second.HUE();
}

bool higher_HUE( ColorStruct & first, ColorStruct & second )
{
	return first.HUE() > second.HUE();
}

double ColorStruct::saturation()
{
	return ColorStruct::saturation_cast( *this );
}

double ColorStruct::saturation_cast( ColorStruct const & cs )
{
	double const * color = cs.color;
	double max = *std::max_element( color, color + channels);
	double min = *std::min_element( color, color + channels);
	
	if ( max < d0upLim )
		return 0.0;
	else
		return 1.0 - min / max ;
}

double ColorStruct::HUE()
{
	return HUE_cast( *this );
}

double ColorStruct::HUE_cast( ColorStruct const & cs )
{
	double const * color = cs.color;
	double max = *std::max_element( color, color + channels);
	double max_min = max - *std::min_element( color, color + channels);
    
    double hue = 0;
    if( max_min < d0upLim )
    	return hue;

    if( color[ 0 ] == max )
      	hue = -120 + ( ( color[ 2 ] - color[ 1 ] ) *60 /( max_min ) );
    if( color[ 1 ] == max)
      	hue = 120 + ( ( color[ 0 ] - color[ 2 ] ) *60 /( max_min ) );
	if( color[ 2 ] == max )
      	hue = 000 + ( ( color[ 1 ] - color[ 0 ] ) *60 /( max_min ) );
	
	if( hue < -180 + baseLevel )
		hue += 360;

	return hue;
}

ColorBalance::ColorBalance( cv::Mat const & img, TYPE acceptanceLevel_, uint distance = 1 ):
img( img ), distance{ distance } //, colorBalance{ ColorStruct{ 0, 0 , 0 } }
{
	acceptanceLevel = std::max( acceptanceLevel_, static_cast< TYPE >( 1 ) );
}

void ColorBalance::balance( std::vector< IndexTransition > const & positions )
{
	std::for_each( positions.begin(), positions.end(), [ this ]( IndexTransition const & el )
	{
		push_element_balance( el );
	});
	
	DataProcess::outliner<double>( colorBalance, 1, both,
							ColorStruct::less_saturation,
							ColorStruct::add_saturation,
							ColorStruct::subtract_saturation,
							ColorStruct::saturation_cast );
	
	ColorStruct sumBalance;

	std::for_each( colorBalance.begin(), colorBalance.end(), [ &sumBalance ]( ColorStruct const & el )
	{
		sumBalance += el;
	} );

	double normalizer = ( sumBalance.color[0] + sumBalance.color[1] + sumBalance.color[2] ) / channels;

	sumBalance /= colorBalance.size() * normalizer;
}

void ColorBalance::push_element_balance( IndexTransition const & shadow )
{
	Transition const & shtransition = shadow.transition;
	if( !is_valid( shadow.transition ) )
		return;

	uint brightRow = shadow.row;
	uint brightCol = shadow.col;
	
	if( shtransition & ( Transition::upToDw ) )		{ brightRow -= distance; }
	if( shtransition & ( Transition::lToR ) )		{ brightCol -= distance * channels; }
	if( shtransition & ( Transition::dwToUp ) )		{ brightRow += distance; }
	if( shtransition & ( Transition::rToL ) )		{ brightCol += distance * channels; }

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

#ifdef WITH_TESTS
	ColorStruct ColorBalance::getColorBalance( uint idx )
	{
		if( colorBalance.size() )
		{
			return colorBalance[ idx ];
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

double ColorStruct::baseLevel = 0.0;

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

double DataProcess::hue_base_level( std::vector< ColorStruct > const & colorBalance )
{
	static double _baseLevel = 0.0;
	static int counter = 2;//two iterations enough
	if ( counter == 0)
		return _baseLevel;
	else
	{
		ColorStruct avg;
		for_each( colorBalance.begin(), colorBalance.end(), [&avg]( ColorStruct const & cs ){ avg += cs;});
		avg /= colorBalance.size();
		_baseLevel = avg.HUE( );
		ColorStruct::baseLevel = _baseLevel;

		hue_base_level( colorBalance );
		--counter;
	}
}

void DataProcess::remove_noise_matches( std::vector<IndexTransition>&  data )
{
	//detect if double side transition aka noise transition
	data.erase(std::remove_if(data.begin(), data.end(), [](auto idxt){
		
		bool result;
		Transition const tr = idxt.transition;
		Transition tmpTr;
		
		tmpTr =	(Transition)( tr & biUpDw );
		result = ( tmpTr == ( tmpTr | biUpDw ) );

		tmpTr =(Transition)( tr & biLR );
		result |= ( tmpTr == ( tmpTr | biLR ) );
		
		return result;

	}), data.end() );
}

template< class TypeIn, class TYPE, class Compare, class BaseArithm, class Cast >
void
DataProcess::outliner( std::vector<TYPE> & dataset, double diffMult, SideToClear side, Compare less, BaseArithm add, BaseArithm subtract, Cast cast_arithm_arg )
{
    TypeIn median( 0 );
    TypeIn Q1( 0 );
    TypeIn Q3( 0 );
    TypeIn diff( 0 );
    TypeIn downLim( 0 );
    TypeIn upLim( 0 );
    double const d_2_0 = 2.0;
    std::sort( dataset.begin(), dataset.end(), less );

    if( ( dataset.size() % 2 ) == 0 )
        median = cast_arithm_arg( dataset[ dataset.size() / 2 ] );
    else
        median = add( dataset[ static_cast< uint >( dataset.size() / d_2_0 ) ], dataset[ static_cast< uint >( dataset.size() / d_2_0 - 1.0 + 0.5 ) ] ) / d_2_0;

    if( ( dataset.size() % 4 ) == 0 )
        Q1 = cast_arithm_arg( dataset[ dataset.size() / 4 ] );
    else
        Q1 = add( dataset[ static_cast< uint >( dataset.size() / 4.0 + 0.5 ) ], dataset[ static_cast< uint >( dataset.size() / 4.0 - 1 + 0.5 ) ] ) / d_2_0;

    if( ( ( dataset.size() * 3 ) % 4 ) == 0 )
        Q3 = cast_arithm_arg( dataset[ dataset.size() * 3 / 4 ] );
    else
        Q3 = add( dataset[ static_cast< uint >( dataset.size() * 3.0/4.0 + 0.5 ) ], dataset[ static_cast< uint >( dataset.size() * 3.0/4.0 - 1 + 0.5 ) ] ) / d_2_0;

    diff = Q3 - Q1;
    diff *= diffMult;
    downLim = Q1 - diff;
    upLim = Q3 + diff;

    //higher upLim;
    uint begg = 0;
    for( begg = dataset.size() - 1; ; begg--) //dataset.size -1 == dataser.end
    {
        if( cast_arithm_arg( dataset[ begg ] ) > upLim )
            continue;
        else
            break;
    }

    uint endd = 0;
    for(endd = 0; ; endd++ )
    {
        if( cast_arithm_arg( dataset[ endd ] ) < downLim )
            continue;
        else
            break;
    }
    
    if( side & SideToClear{ tail } )
    	dataset.resize( begg + 1 );//rm tail

    if( side & SideToClear{ head } )
    	dataset.erase( dataset.begin(), dataset.begin() + endd );//rm head
}