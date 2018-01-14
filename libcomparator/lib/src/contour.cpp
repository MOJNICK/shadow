#include "contour.hpp"

ContourTransition::ContourTransition( cv::Mat& image )
{
	copy_data( image );
}

void ContourTransition::bw_push_transition(std::vector<IndexTransition> const & indexTransition)
{
	std::for_each(indexTransition.begin(), indexTransition.end(), [ this ](auto& el){
		matTransition( el.row, el.col ).transition = el.transition;
    });
}

void ContourTransition::copy_data( cv::Mat& image )
{
	set_transition_to_no();
	matTransition.create( image.rows, image.cols );
	for(int row = 0; row < matTransition.rows; row++ )
		for(int col = 0; col < matTransition.cols; col++ )
		{
			matTransition(row, col).pixel = *image.ptr(row, col);
		}
}

void ContourTransition::set_transition_to_no()
{
	for(int row = 0; row < matTransition.rows; row++ )
		for(int col = 0; col < matTransition.cols; col++ )
		{
			matTransition(row, col).transition = Transition::no;
		}
}


Preprocess::Preprocess( cv::Mat_<double> filterKernel_, cv::Mat srcImg_ ) : srcImgSize(srcImg_.rows, srcImg_.cols)
{
	filterKernel = filterKernel_.clone();
}

cv::Mat Preprocess::get_thick_kernel( cv::Mat& image, int dilationSize )
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

	return thickKernel;
}

ContourTransition Preprocess::get_correction_edge( cv::Mat const & thickKernel, std::vector<IndexTransition> const & indexTransition )
{
	if(filterKernel.rows % 2 != 0 | filterKernel.cols % 2 != 0 )
		std::cout << "\nnot even filterKernel Preprocess::get_correction_edge\n";

	cv::Mat thickKernelBorder;
	cv::copyMakeBorder( thickKernel, thickKernelBorder, filterKernel.rows / 2, filterKernel.rows / 2, filterKernel.cols / 2, filterKernel.cols / 2, cv::BORDER_REFLECT);//to be safe
	cv::Rect rct( filterKernel.cols, filterKernel.rows, thickKernel.cols + filterKernel.cols, thickKernel.rows + filterKernel.rows);//TBD
	cv::Mat roi = thickKernelBorder(rct);

}

cv::Mat_<Transition> Preprocess::cvt_it_to_matT( std::vector<IndexTransition> const & indexTransition )
{
	cv::Mat_<Transition> result;
	cv::Mat_<Transition> tmpResult;
	std::for_each( indexTransition.begin(), indexTransition.end(), [&tmpResult]( auto& el){
		tmpResult( el.row, el.col) = el.transition;
	});
	cv::copyMakeBorder( tmpResult, result, filterKernel.rows / 2, filterKernel.rows / 2, filterKernel.cols / 2, filterKernel.cols / 2, cv::BORDER_REFLECT);//to be safe

	return result;
}

cv::Mat_<double> MakeFilter::get_square_filter(int size)
{
	if( size % 2 == 0)
		size -= 1;

	cv::Mat_<double> result( cv::Size(size, size), 1/( size * size ));
	return result;
}