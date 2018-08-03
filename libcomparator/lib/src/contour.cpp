#include "contour.hpp"
#include "imageprocess.hpp"

ContourTransition::ContourTransition( cv::Mat& image )
{
	copy_data( image );
	set_transition_to_no();
}

cv::Mat ContourTransition::show_matDataTrans()
{
	cv::Mat blackImage(matDataTrans.rows, matDataTrans.cols, CV_8UC3, cv::Scalar(0,0,0));
	
	for(int row = 0; row < matDataTrans.rows; ++row )
		for(int col = 0; col < matDataTrans.cols; ++col )
		{
			Transition trans = matDataTrans(row, col).transition;
			if(trans & lToR)
			{
				blackImage.at<cv::Vec3b>(row, col) = cv::Vec3b(255, 0, 0);
			}
			if(trans & rToL)
			{
				blackImage.at<cv::Vec3b>(row, col) = cv::Vec3b(0, 255, 0);
			}
			if(trans & upToDw)
			{
				blackImage.at<cv::Vec3b>(row, col) = cv::Vec3b(0, 0, 255);
			}
			if(trans & dwToUp)
			{
				blackImage.at<cv::Vec3b>(row, col) = cv::Vec3b(0, 160, 255);
			}
			if(trans & unknown)
			{
				blackImage.at<cv::Vec3b>(row, col) = cv::Vec3b(255, 255, 255);
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

/*remove shadow transitions when not under canny*/
void Preprocess::rm_out_edge_detected( std::vector<IndexTransition> & indexTransition )
{
	indexTransition.erase( std::remove_if(indexTransition.begin(), indexTransition.end(), [ this ]( auto& idxTr){
		if( thickKernel.at<unsigned char>( idxTr.row, idxTr.col) == 0 )
			return true;
		else
			return false;

	}), indexTransition.end());
}

/*canny and dilate*/
cv::Mat Preprocess::make_thick_kernel( cv::Mat const & image, uint dilationSize )//tested visually
{
	cv::Mat gray, edge;
    
    cvtColor(image, gray, CV_BGR2GRAY);
    blur( gray, gray, cv::Size(7, 7) );

    int apertureSize = 5;
    Canny( gray, edge, 80, 500, apertureSize, true);//
 
    edge.convertTo(thickKernel, CV_8U);

    cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                                       cv::Size( 2*dilationSize + 1, 2*dilationSize+1 ),
                                       cv::Point( dilationSize, dilationSize ) );
  	morphologyEx( thickKernel, thickKernel, cv::MORPH_DILATE, element, cv::Point(-1,-1), 1, cv::BORDER_REFLECT );
	
	return thickKernel;
}

ContourTransition Preprocess::get_correction_edge( cv::Mat const & image, std::vector<IndexTransition> const & indexTransition, uint dilationSize )
{
	cv::Mat kernelSilentBorder;
	cv::copyMakeBorder( make_thick_kernel( image, dilationSize ), kernelSilentBorder, filterKernel.rows / 2, filterKernel.rows / 2, filterKernel.cols / 2, filterKernel.cols / 2, cv::BORDER_REFLECT);

	// cv::Mat kernelSilentBorderROI = kernelSilentBorder(rct);
	cv::Mat_<Transition> matTransSilentBorder = cvt_it_to_matTSilent( indexTransition );//silent border inside

	cv::Rect rct( filterKernel.cols / 2 + 1, filterKernel.rows / 2 + 1, image.cols - filterKernel.cols / 2 + 1, image.rows - filterKernel.rows / 2 + 1 );
	ContourTransition contourTransition( kernelSilentBorder );//full , not roi //::no //v<0;255>
	contourTransition.matDataTrans( rct );//silent border
	for( int row = 0; row < contourTransition.matDataTrans.rows; ++row )
	{
		for( int col = 0; col < contourTransition.matDataTrans.cols; ++col )
		{
			if( contourTransition.matDataTrans.at<DataTransition>( row, col ).pixel == (unsigned char)(255) )
			{
				contourTransition.matDataTrans.at<DataTransition>( row, col ).transition = get_direction( row, col, matTransSilentBorder);
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
			#ifdef WITH_TESTS
			auto debugValue = (matTransSilentBorder(_row, _col) & all) >> shiftToDistinct;
			if( (debugValue) >= transDirCombi )
				std::cout << "\n\n\nERRR\n\n\n";
			#endif

			histo[ (matTransSilentBorder(_row, _col) & all) >> shiftToDistinct ].first += filterKernel(_row - row + filterKernel.rows / 2, _col - col + filterKernel.cols / 2 );
		}
	histo[0].first = 0;

	std::pair<double, uint> orgHisto[ transDirCombi ];
	std::copy( histo, histo + transDirCombi, orgHisto );
	for( int i = 0; i < transDirCombi; ++i )//concatenate similar transitions
	{
		histo[i].first = 0;
		for( int j = 0; j < transDirCombi; ++j )
		{
			if( (i & j) == i )
			{
				histo[i].first += orgHisto[j].first;
			}
		}
	}

	std::sort(histo, histo + transDirCombi, std::greater<std::pair<double, uint>>());
	
	if( histo[0].first / histo[1].first > 2 )
	{

		result = static_cast<Transition>( histo[0].second << shiftToDistinct );
		if( is_noise_detection( result ))
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
		if( is_noise_detection( result ))
		{
			return Transition::unknown;
		}
		else
		{
			return result;
		}
	}
}


cv::Mat_<Transition> Preprocess::cvt_it_to_matTSilent( std::vector<IndexTransition> const & indexTransition )
{
	cv::Mat_<Transition> result( srcImgSize);//, Transition::no );
	for( int i = 0; i < result.total(); ++i){ ((Transition*)(result.data))[i] = Transition::no; }

	std::for_each( indexTransition.begin(), indexTransition.end(), [&result]( auto& el){
		result.at<Transition>( el.row, el.col) = el.transition;
	});
	cv::copyMakeBorder( result, result, filterKernel.rows / 2, filterKernel.rows / 2, filterKernel.cols / 2, filterKernel.cols / 2, cv::BORDER_REFLECT);//to be safe

	return result;//return with silent border
}

cv::Mat MakeFilter::box_kernel( int height, int width )
{
	height |= 1;

	cv::Mat result( cv::Size( height, height ), CV_64F, 1.0/( height * height ) );

	return result;
}

cv::Mat MakeFilter::calc_antisimmetric_filter( double sigma, double sizeFactor, Transition direction, double hvFactor )//add direction factor
{
	if(!(direction && !(direction & (direction - 1))))
		return cv::Mat( 1, 1, CV_64F, 1.0 );

	assert( std::abs(hvFactor) > 0.01 );

	if( direction & biUpDw )
		hvFactor = 1 / hvFactor;		

	int sigmaH = (int)(sigma * hvFactor) | 1;
	int sigmaV = (int)(sigma / hvFactor) | 1;
	int sizeH = (int)(sigmaV * sizeFactor) | 1;
	int sizeV = (int)(sigmaV * sizeFactor) | 1;

	cv::Mat kernel;
	switch(kernelType)
	{
		case KernelType::gauss : kernel = gauss_kernel( cv::Size(sigmaH, sigmaV), cv::Size(sizeH, sizeV) ); break;
		case KernelType::triangle : kernel = triangle_kernel( cv::Size(sigmaH, sigmaV) ); break;
	}
 	cvt_to_antisimmetric( kernel, direction );

	return kernel;
}

cv::Mat MakeFilter::gauss_kernel( cv::Size kernelSigma, cv::Size kernelSize )
{
	cv::Mat kernel( cv::Size( kernelSize.height, kernelSize.width ), CV_64F, .0 );
	int anchorH = kernel.rows / 2;
	int anchorV = kernel.cols / 2; //not exactly	
	kernel.at<double>(anchorH, anchorV) = 1.0;	

	cv::GaussianBlur( kernel, kernel, kernelSize, kernelSigma.width , kernelSigma.height );
	return kernel;
}

cv::Mat MakeFilter::triangle_kernel( cv::Size kernelSigma, cv::Size kernelSize )
{
	int sigma = kernelSigma.height;
	sigma |= 1;
	int border = sigma / 2;
	cv::Mat boxKernel = box_kernel( sigma );
	copyMakeBorder(boxKernel, boxKernel, border, border, border, border, cv::BORDER_CONSTANT, cv::Scalar(0) );
	cv::Mat triangleKernel;
	cv::filter2D( boxKernel, triangleKernel, -1, boxKernel, cv::Point(-1,-1), 0, cv::BORDER_REFLECT );

	return triangleKernel;
}

void MakeFilter::cvt_to_antisimmetric(cv::Mat& kernel, Transition direction, int anchorH, int anchorV)
{
	if(anchorH == -1)
	{
		anchorH = kernel.rows / 2;
		anchorV = kernel.cols / 2; //not exactly
	}

	for(int i = 0; i < kernel.rows; i++)
	{
	    for(int j = 0; j < kernel.cols; j++)
	    {
	    	if( direction == upToDw)
	    		if( i < anchorV )
	        		kernel.at<double>(i, j) = -kernel.at<double>(i, j);

	    	if( direction == lToR)
	    		if( j < anchorH )
	        		kernel.at<double>(i, j) = -kernel.at<double>(i, j);

	    	if( direction == dwToUp)
	    		if( i > anchorV )
	        		kernel.at<double>(i, j) = -kernel.at<double>(i, j);

	    	if( direction == rToL)
	    		if( j > anchorH )
	        		kernel.at<double>(i, j) = -kernel.at<double>(i, j);
	    }
	}
}


Filter::Filter( cv::Mat & image, std::vector<IndexTransition> const & indexTransition,
				double sizeFactor, double antiSigma, double hvFactor,
				uint calcDistance, FilterMode filterMode )
:
	srcImgSize( image.cols, image.rows ), _image(image), sizeFactor{sizeFactor}, antiSigma{antiSigma}, hvFactor{hvFactor}, filterMode{filterMode}
{
	calc_correction_power( indexTransition, calcDistance );
	get_shadow_weight( indexTransition );
}

cv::Vec3d Filter::calc_correction_power( std::vector<IndexTransition> const & indexTransition, uint calcDistance )
{
	ColorBalance colorBalance(_image, 5u, calcDistance);
	ColorStruct colorStruct = colorBalance.balance( indexTransition, BalanceMode::brightness );
	for(int i = 0; i < channels; ++i){ correctionPower[i] = colorStruct.get_color( i ); }
}

void Filter::get_shadow_weight( std::vector<IndexTransition> const & indexTransition )
{
	cv::Mat directed = cvt_it_to_matFloat( indexTransition );
	std::vector<cv::Mat> splited;
	cv::split(directed, splited);

	MakeFilter calc_antisimmetric_filter;
	cv::Mat UPkernel = calc_antisimmetric_filter( antiSigma, sizeFactor, upToDw, hvFactor);
	cv::Mat Lkernel = calc_antisimmetric_filter( antiSigma, sizeFactor, lToR, hvFactor);
	cv::Mat DWkernel = calc_antisimmetric_filter( antiSigma, sizeFactor, dwToUp, hvFactor);
	cv::Mat Rkernel = calc_antisimmetric_filter( antiSigma, sizeFactor, rToL, hvFactor);

	cv::filter2D( splited[0], splited[0], -1, UPkernel, cv::Point(-1,-1), 0, cv::BORDER_ISOLATED );
	cv::filter2D( splited[1], splited[1], -1, Lkernel, cv::Point(-1,-1), 0, cv::BORDER_ISOLATED );
	cv::filter2D( splited[2], splited[2], -1, DWkernel, cv::Point(-1,-1), 0, cv::BORDER_ISOLATED );
	cv::filter2D( splited[3], splited[3], -1, Rkernel, cv::Point(-1,-1), 0, cv::BORDER_ISOLATED );

	reverseZeroBasedFilter = splited[0] + splited[1] + splited[2] + splited[3];
		
	if(filterMode == FilterMode::grabCut)
	{
		auto minmax = std::minmax_element(reverseZeroBasedFilter.begin<double>(), reverseZeroBasedFilter.end<double>());
		double min = *minmax.first;
		double max = *minmax.second;
		min*=0.3; max*=0.5;

		cv::Mat mask = reverseZeroBasedFilter.clone();
		cv::GaussianBlur( mask, mask, cv::Size(7,7), 4 , 4 );
		std::for_each(mask.begin<double>(), mask.end<double>(), 
			[min, max](auto& el){
				if(el < min)
				{
					el = cv::GC_FGD;
				}
				else if(el < 0)
				{
					el = cv::GC_PR_FGD;
				}
				else if(el < max)
				{
					el = cv::GC_PR_BGD;
				}
				else
				{
					el = cv::GC_BGD;
				}
			}
		);

		mask.convertTo(mask, CV_8U);
		#ifdef VERBOSE
		present_grabCut_mask(mask);
		#endif
		cv::Mat bgdModel, fgdModel;
		cv::grabCut( _image, mask, cv::Rect(0, 0, mask.cols, mask.rows), bgdModel, fgdModel, 5, cv::GC_INIT_WITH_MASK );
		for(int i=0; i<mask.rows*mask.cols; ++i)
		{
			uchar maskValue = mask.data[i];
			if( maskValue == cv::GC_FGD || maskValue == cv::GC_PR_FGD)
			{
				*(reinterpret_cast<double*>(reverseZeroBasedFilter.data) + i) = -1.0;
			}
			else
			{
				*(reinterpret_cast<double*>(reverseZeroBasedFilter.data) + i) = 1.0;
			}
		}

		#ifdef VERBOSE
		int occurences[3] = {0};
		std::for_each(reverseZeroBasedFilter.begin<double>(), reverseZeroBasedFilter.end<double>(), 
			[&occurences](auto& el){
				if(el == -1.0)
				{
					occurences[0]++;
				}
				else if(el == 1.0)
				{
					occurences[1]++;
				}
				else
				{
					occurences[2]++;
				}
			}
		);

		for(int i =0; i<2; ++i)
		{
			std::cout<<occurences[i] << '\n';
		}
		present_grabCut_mask(mask);

		#endif
	}
	return;
}

cv::Mat Filter::filter_image()
{
	for(int i = 0; i < _image.rows; i++)
	{	
	    for(int j = 0; j < _image.cols; j++)
	    {	
	    	if(reverseZeroBasedFilter.at<double>(i, j) < .0)
	    	{
			    cv::Vec3b vec3b = _image.at<cv::Vec3b>( i, j );
				cv::multiply( vec3b, correctionPower, vec3b);
				_image.at<cv::Vec3b>( i, j ) = vec3b;
	    	}
		}
	}
	return _image;
}

cv::Mat Filter::cvt_it_to_matFloat( std::vector<IndexTransition> const & indexTransition )
{
	cv::Mat result( srcImgSize, CV_64FC4, .01 );//, Transition::no );

	std::for_each( indexTransition.begin(), indexTransition.end(), [&result]( auto& el){
		cv::Vec4d vec4d = result.at<cv::Vec4d>( el.row, el.col);//reference
		vec4d[ 0 ] = el.transition & upToDw && 1;
		vec4d[ 1 ] = el.transition & lToR && 1;
		vec4d[ 2 ] = el.transition & dwToUp && 1;
		vec4d[ 3 ] = el.transition & rToL && 1;
		result.at<cv::Vec4d>( el.row, el.col) = vec4d;
	});

	return result;//return with silent border
}