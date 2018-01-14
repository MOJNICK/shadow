#include <opencv2/core/core.hpp>
#include "libcomparator.hpp"

class MatTransition
{
	unsigned char pixel;
	Transition transition;
}

class ContourTransition
{
public:
	ContourTransition( cv::Mat& image );

	cv::Mat<MatTransition> matTransition;
	void bw_push_transition(std::vector<IndexTransition> const & indexTransition);
}

class Preprocess
{
public:
	cv::Mat get_thick_kernel( cv::Mat& image );
}