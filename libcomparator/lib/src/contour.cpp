#include "contour.hpp"

ContourTransition::ContourTransition( cv::Mat& image )
{

}

void ContourTransition::bw_push_transition(std::vector<IndexTransition> const & indexTransition)
{
	std::for_each(result.begin(), result.end(), [ this ](auto el){
        this->matTransition.data[el.bw_index( this->matTransition )].transition = el.transition;
    });
}

cv::Mat Preprocess::get_thick_kernel( cv::Mat& image )
{
	thickKernel = image.clone();
	return thickKernel;
}