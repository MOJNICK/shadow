#include <opencv2/core/core.hpp>
#include "libcomparator.hpp"

class DataTransition
{
public:
	unsigned char pixel;
	Transition transition;
};

class ContourTransition
{
public:
	cv::Mat_<DataTransition> matTransition;
	
	ContourTransition( cv::Mat& image );
	void bw_push_transition(std::vector<IndexTransition> const & indexTransition);

private:
	void copy_data( cv::Mat& image );
	void set_transition_to_no();
};

class Preprocess
{
public:
	cv::Mat get_thick_kernel( cv::Mat& image );
};