#include "contour.hpp"

ContourTransition::ContourTransition( cv::Mat& image )
{
	copy_data( image );
	set_transition_to_no();
}

cv::Mat ContourTransition::show_matDataTrans()
{
	cv::Mat blackImage(matDataTrans.rows, matDataTrans.cols * channels, CV_8UC3, cv::Scalar(0,0,0));
	
	for(int row = 0; row < matDataTrans.rows; row++ )
		for(int col = 0; col < matDataTrans.cols; col+=3 )
		{
			switch (matDataTrans(row, col).transition )
			{
				case lToR : *blackImage.ptr(row, col + 1) = 255; break;
				case upToDw : *blackImage.ptr(row, col + 2) = 255; break;
				default : *blackImage.ptr(row, col + 1) = 64;
			}
		}

	return blackImage;
}

void ContourTransition::bw_push_transition(std::vector<IndexTransition> const & indexTransition)
{
	std::for_each(indexTransition.begin(), indexTransition.end(), [ this ](auto& el){
		matDataTrans( el.row, el.col ).transition = el.transition;
    });
}

void ContourTransition::copy_data( cv::Mat& image )
{
	set_transition_to_no();
	matDataTrans.create( image.rows, image.cols );
	for(int row = 0; row < matDataTrans.rows; row++ )
		for(int col = 0; col < matDataTrans.cols; col++ )
		{
			matDataTrans(row, col).pixel = *image.ptr(row, col);
		}
}

void ContourTransition::set_transition_to_no()
{
	for(int row = 0; row < matDataTrans.rows; row++ )
		for(int col = 0; col < matDataTrans.cols; col++ )
		{
			matDataTrans(row, col).transition = Transition::no;
		}
}


Preprocess::Preprocess( cv::Mat_<double> filterKernel_, cv::Mat const & image ) : srcImgSize(image.rows, image.cols)
{
	if(filterKernel_.rows & 1 == 0 | filterKernel_.cols & 1 == 0 )
		std::cout << "\nnot symmetric filterKernel_ Preprocess::get_correction_edge\n";
	
	filterKernel = filterKernel_.clone();
}

cv::Mat Preprocess::get_thick_kernel( cv::Mat const & image, uint dilationSize )//tested visually
{
	cv::Mat thickKernel = image.clone();
 
    cv::Mat gray, edge, draw;
    cvtColor(image, gray, CV_BGR2GRAY);
 
    blur( gray, gray, cv::Size(7, 7) );

    int apertureSize = 5;
    Canny( gray, edge, 80, 500, apertureSize, true);//
 
    edge.convertTo(thickKernel, CV_8U);

    //dilate here
    cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                                       cv::Size( 2*dilationSize + 1, 2*dilationSize+1 ),
                                       cv::Point( dilationSize, dilationSize ) );
  /// Apply the dilation operation
  	dilate( thickKernel, thickKernel, element );

  	cv::Mat kernelSilentBorder;
	cv::copyMakeBorder( thickKernel, kernelSilentBorder, filterKernel.rows / 2, filterKernel.rows / 2, filterKernel.cols / 2, filterKernel.cols / 2, cv::BORDER_REFLECT);//to be safe
	

	return kernelSilentBorder;
}

ContourTransition Preprocess::get_correction_edge( cv::Mat const & image, std::vector<IndexTransition> const & indexTransition, uint dilationSize )
{
	cv::Mat kernelSilentBorder = get_thick_kernel( image, dilationSize );//thick canny

	// cv::Mat kernelSilentBorderROI = kernelSilentBorder(rct);
	cv::Mat_<Transition> matTransSilentBorder = this->cvt_it_to_matT( indexTransition );//silent border inside

	cv::Rect rct( filterKernel.cols / 2 + 1, filterKernel.rows / 2 + 1, image.cols - filterKernel.cols / 2 + 1, image.rows - filterKernel.rows / 2 + 1 );
	ContourTransition contourTransition( kernelSilentBorder );//full , not roi //::no //v<0;255>
	contourTransition.matDataTrans( rct );//silent border
	for( int row = 0; row < contourTransition.matDataTrans.rows; ++row )
	{
		for( int col = 0; col < contourTransition.matDataTrans.cols; ++col )
		{
			if( contourTransition.matDataTrans( row, col ).pixel == (unsigned char)(255) )
			{
				contourTransition.matDataTrans( row, col ).transition = get_direction( row, col, matTransSilentBorder);
			}
		}
	}
	return contourTransition;
}

Transition Preprocess::get_direction( int const row, int const col, cv::Mat_<Transition> matTransSilentBorder )//core method
{
	Transition result{ empty };
	std::pair<double, uint> histo[ transDirCombi ];
	for( int i = 0; i < transDirCombi; i++ )
	{
		histo[i] = std::pair<double, uint>( 0.0, i );
	}

	for(int _row = row - filterKernel.rows / 2; _row <= row + filterKernel.rows / 2; _row++ )
		for(int _col = col - filterKernel.cols/2; _col <= col + filterKernel.cols / 2; _col++ )
		{
			auto debugValue = (matTransSilentBorder(_row, _col) & all) >> shiftToDistinct;
			if( (debugValue) >= transDirCombi )
				std::cout << "\n\n\nERRR\n\n\n";

			histo[ (matTransSilentBorder(_row, _col) & all) >> shiftToDistinct ].second += filterKernel(_row - row + filterKernel.rows / 2, _col - col + filterKernel.cols / 2 );
		}


	std::pair<double, uint> orgHisto[ transDirCombi ];
	std::copy( histo, histo + transDirCombi, orgHisto );
	for( int i = 0; i < transDirCombi; ++i )//concatenate similar transition probability
	{
		for( int j = 0; j < transDirCombi; ++j )
		{
			if( i & j == i )
			{
				histo[i].first += orgHisto[j].first;
			}
		}
	}

	std::sort(histo, histo + transDirCombi, std::greater<std::pair<double, uint>>());
	
	if( histo[0].first / histo[1].first > 2 )
	{

		result = static_cast<Transition>( histo[0].second << shiftToDistinct );
		if( DataProcess::is_noise_detection( result ))
		{
			return Transition::unknown;
		}
		else
		{
			return result;
		}
	}
	else
	{
		result = static_cast<Transition>( ( histo[0].second | histo[1].second )<< shiftToDistinct );
		if( DataProcess::is_noise_detection( result ))
		{
			return Transition::unknown;
		}
		else
		{
			return result;
		}
	}
}


cv::Mat_<Transition> Preprocess::cvt_it_to_matT( std::vector<IndexTransition> const & indexTransition )
{
	cv::Mat_<Transition> result;
	cv::Mat_<Transition> tmpResult( srcImgSize);//, Transition::no );
	for( int i = 0; i < tmpResult.total(); ++i){ ((Transition*)(tmpResult.data))[i] = Transition::no; }
	// for( int i = 0; i < tmpResult.total(); ++i)//debug
	// {
	// 	//((double*)(tmpResult.data))[i] = 1.0/( filterSize * filterSize );
	// 	std::cout << tmpResult(0 , i);
	// }
	// std::cout<<std::endl;

	std::for_each( indexTransition.begin(), indexTransition.end(), [&tmpResult]( auto& el){
		tmpResult( el.row, el.col) = el.transition;
	});
	cv::copyMakeBorder( tmpResult, result, filterKernel.rows / 2, filterKernel.rows / 2, filterKernel.cols / 2, filterKernel.cols / 2, cv::BORDER_REFLECT);//to be safe

	return result;//return with silent border
}

cv::Mat_<double> MakeFilter::get_square_filter( int filterSize )
{
	if( filterSize & 1 == 0)
		filterSize -= 1;

	cv::Mat_<double> result( cv::Size2i( filterSize, filterSize ), 1.0/( filterSize * filterSize ) );

	return result;
}