#ifndef PROCESS_2D_HPP
#define PROCESS_2D_HPP

#define MASK_PROCESS
#include "imageprocess.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "contour.hpp"

#ifdef WITH_TESTS
	#include <opencv2/highgui/highgui.hpp>
#endif

namespace process2d
{
	cv::Mat run_process2d(cv::Mat image, cv::Mat image1D);
}
#endif