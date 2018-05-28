#ifndef PROCESS_2D_HPP
#define PROCESS_2D_HPP
#define MASK_PROCESS
#include "libcomparator.hpp"
#include "imageprocess.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#ifdef WITH_TESTS
	#include <opencv2/highgui/highgui.hpp>
#endif

double balance[] = {1.0, 1.0, 1.0};

class MaskIterateProcess
{
public:
	IterateProcessMask<TYPE> iterateProcessMask;
	MaskIterateProcess(cv::Mat image, cv::Mat mask)
	:
		iterateProcessMask
		(
			image,
			std::numeric_limits<TYPE>::lowest(),
			std::numeric_limits<double>::min(),
			std::numeric_limits<double>::max(),
			balance,
			mask
		)
	{}
	std::vector<IndexTransition> calc_transitions_with_mask()
	{
		return iterateProcessMask.iterate_HV();
	}
private:
};

namespace Process2D
{

	cv::Mat calc_canny( cv::Mat image, int blurr = 7, int threshold1 = 80, int threshold2 = 500, int apertureSize = 5 )
	{
		cv::Mat edge;
		cvtColor( image, edge, CV_BGR2GRAY );
 
	    blur( edge, edge, cv::Size(blurr, blurr) );

	    Canny( edge, edge, threshold1, threshold2, apertureSize, true );

	    edge.convertTo(edge, CV_8U);

	    #ifdef WITH_TESTS
	    cv::imshow( "Canny", edge );
	    #endif

	    return edge;
	}

	cv::Mat run_process2D(cv::Mat image, cv::Mat image_1d)
	{
		cv::Mat img;
		img = image.clone();
		return image;
	}

}
#endif