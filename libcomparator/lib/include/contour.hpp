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
	cv::Mat_<DataTransition> matDataTrans;
	
	ContourTransition( cv::Mat& image );

private:
	void copy_data( cv::Mat& image );
	void set_transition_to_no();
	void bw_push_transition(std::vector<IndexTransition> const & indexTransition);
};

class Preprocess
{
public:
	Preprocess( cv::Mat_<double> filterKernel_, cv::Mat srcImg_ );

	cv::Mat get_thick_kernel( cv::Mat& image, int dilationSize );
	ContourTransition get_correction_edge( cv::Mat const & thickKernel, std::vector<IndexTransition> const & indexTransition );
private:
	cv::Mat_<double> filterKernel;
	cv::Size2i srcImgSize;

	DataTransition get_direction( int row, int col, cv::Mat_<Transition> matTrans );
	// DataTransition set_direction( cv::Mat thickKernelROI, cv::Mat_<Transition> matTrans );
	cv::Mat_<Transition> cvt_it_to_matT( std::vector<IndexTransition> const & indexTransition );
};

class MakeFilter
{
public:
	static cv::Mat_<double> get_square_filter( int size );
private:
	// cv::Mat_<double> cvt_mat_matDoble(cv::Mat filterKernel);
};