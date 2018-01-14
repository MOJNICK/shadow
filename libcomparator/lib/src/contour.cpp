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



cv::Mat Preprocess::get_thick_kernel( cv::Mat& image )
{
	cv::Mat thickKernel = image.clone();
	return thickKernel;
}