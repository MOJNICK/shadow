#include "process_2d.hpp"
#include "libcomparator.hpp"

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

namespace process2d
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

	cv::Mat run_process2d(cv::Mat image, cv::Mat image1D)
	{
		assert( image.cols == image1D.cols && image.cols == image1D.cols );
		
		cv::Mat cannyImage = calc_canny(image);
		cv::Mat cannyImage1D = calc_canny(image1D);

		cv::Mat shadowEdges;
		cv::subtract(cannyImage, cannyImage1D, shadowEdges);

		//auto idTrCluster = test_on_image( path, factor, 3.0, 1 );
		MaskIterateProcess maskIterateProcess( image, shadowEdges);
		auto idTr = maskIterateProcess.calc_transitions_with_mask();

    	Filter filter(image, idTr, 160, 3, 1);
    	cv::Mat result = filter.filter_image();

		return result;
	}

}