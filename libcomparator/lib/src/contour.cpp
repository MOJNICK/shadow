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



cv::Mat Preprocess::get_thick_kernel( cv::Mat& image, int dilationSize )
{
	cv::Mat thickKernel = image.clone();
 
    cv::Mat gray, edge, draw;
    cvtColor(image, gray, CV_BGR2GRAY);
 
    blur( gray, gray, cv::Size(10,10) );

    Canny( gray, edge, 10, 40, 3);
 
    edge.convertTo(thickKernel, CV_8U);

    //dilate here
    cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                                       cv::Size( 2*dilationSize + 1, 2*dilationSize+1 ),
                                       cv::Point( dilationSize, dilationSize ) );
  /// Apply the dilation operation
  	dilate( thickKernel, thickKernel, element );

	return thickKernel;
}