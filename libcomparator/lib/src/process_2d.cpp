#include "process_2d.hpp"
//#include "libcomparator.hpp"

double balance[] = {1.0, 1.0, 1.0};

class MaskIterateProcess
{
public:
	IterateProcessMask<TYPE> iterateProcessMask;
	
	MaskIterateProcess(cv::Mat image, cv::Mat mask, int compareDistance = 1)
	:
		iterateProcessMask
		(
			image,
			std::numeric_limits<TYPE>::lowest(),
			std::numeric_limits<double>::min(),
			std::numeric_limits<double>::max(),
			balance,
			compareDistance,
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
	int const blurDivisor = 100;

	cv::Mat calc_canny( cv::Mat image, int blurr = 7, int threshold1 = 80, int threshold2 = 500, int apertureSize = 5 )
	{
		cv::Mat edge;
		cv::cvtColor( image, edge, CV_BGR2GRAY );
	    cv::blur( edge, edge, cv::Size(blurr, blurr) );
	    #ifdef WITH_TESTS
	    cv::imshow( "calc_canny", edge );
	    cv::waitKey(0);
	    #endif
	    cv::Canny( edge, edge, threshold1, threshold2, apertureSize, true );
	    edge.convertTo(edge, CV_8U);
	    #ifdef WITH_TESTS
	    cv::imshow( "calc_canny", edge );
	    cv::waitKey(0);
	    #endif
	    return edge;
	}

	bool is_subset(cv::Mat cannyImage1D, cv::Mat const cannyImage, double maximumNonSubsetPercentage = 10)
	{
		cv::Mat cannyImage_ = cannyImage.clone();
		int setSize = cv::countNonZero(cannyImage_);
		
		int dilationSize = 10;
		cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                                       cv::Size( 2*dilationSize + 1, 2*dilationSize+1 ),
                                       cv::Point( dilationSize, dilationSize ) );
  		cv::morphologyEx( cannyImage_, cannyImage_, cv::MORPH_DILATE, element, cv::Point(-1,-1), 1, cv::BORDER_REFLECT );

  		#ifdef WITH_TESTS
  		cv::imshow( "dilatedCannyImage", cannyImage_ );
	    cv::waitKey(0);
  		#endif

  		cv::bitwise_xor(cannyImage_, cannyImage1D, cannyImage_);
  		cv::bitwise_and(cannyImage_, cannyImage1D, cannyImage_);
  		int numberOfNonSubset = cv::countNonZero(cannyImage_);
  		
  		#ifdef WITH_TESTS
  		cv::imshow( "nonSubset", cannyImage_ );
	    cv::waitKey(0);
  		std::cout << "process2d::is_subset::numberOfNonSubset: " << numberOfNonSubset << " setSize: " << setSize << std::endl;
  		#endif

  		return 100.0 * numberOfNonSubset / setSize < maximumNonSubsetPercentage;
	}

	cv::Mat canny_until_1d_is_subset_of_orginal(cv::Mat const cannyImage, cv::Mat image1D, int blurr, int startThreshold, double thresholdDivisor = 1.3, int ratio = 2)
	{
		#ifdef WITH_TESTS
		static int iteration = 10;
		--iteration;
		assert(iteration);
		#endif

		cv::Mat cannyImage1D = calc_canny(image1D, blurr, startThreshold, startThreshold*ratio);
		if(is_subset(cannyImage1D, cannyImage))
		{
			startThreshold/=thresholdDivisor;
			canny_until_1d_is_subset_of_orginal(cannyImage, image1D, blurr, startThreshold);
		}
		else
		{
			startThreshold*=thresholdDivisor;
			cannyImage1D = calc_canny(image1D, blurr, startThreshold, startThreshold*ratio);
			return cannyImage1D;
		}
	}

	cv::Mat run_process2d(cv::Mat image, cv::Mat image1D)
	{
		assert( image.rows == image1D.rows && image.cols == image1D.cols );
		
		int startThreshold = 5;
		int startThreshold1D = startThreshold * 10;
		int blurr = image.cols / blurDivisor;
		cv::Mat cannyImage = calc_canny(image, blurr, startThreshold);
		cv::Mat cannyImage1D = canny_until_1d_is_subset_of_orginal(cannyImage, image1D, blurr, startThreshold1D);
		
		cv::Mat shadowEdges;
		cv::subtract(cannyImage, cannyImage1D, shadowEdges);
		#ifdef WITH_TESTS
	    cv::imshow( "shadowEdges", shadowEdges );
	    cv::waitKey(0);
	    #endif

		//auto idTrCluster = test_on_image( path, factor, 3.0, 1 );
		MaskIterateProcess maskIterateProcess(image, shadowEdges);
		auto idTr = maskIterateProcess.calc_transitions_with_mask();

    	Filter filter(image, idTr, 160, 3, 1);
    	cv::Mat result = filter.filter_image();

		return result;
	}
}