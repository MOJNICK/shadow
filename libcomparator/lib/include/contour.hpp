#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "libcomparator.hpp"
#include <iostream>

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

private:
	void copy_data( cv::Mat& image );
	void set_transition_to_no();
	void bw_push_transition(std::vector<IndexTransition> const & indexTransition);
};

class Preprocess
{
public:
	static cv::Mat get_thick_kernel( cv::Mat& image, int dilationSize );
	static cv::Mat_<DataTransition> get_correction_edge( cv::Mat const & thickKernel, std::vector<IndexTransition> const & indexTransition, cv::Mat filterKernel );
private:
};