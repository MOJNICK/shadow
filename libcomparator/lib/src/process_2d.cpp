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
	    cv::waitKey(0);
	    #endif
	    return edge;
	}

	cv::Mat run_process2d(cv::Mat image, cv::Mat image1D)
	{
		assert( image.rows == image1D.rows && image.cols == image1D.cols );
		
		cv::Mat cannyImage = calc_canny(image, image.cols / 100);
		cv::Mat cannyImage1D = calc_canny(image1D, image.cols / 100);

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

	set_canny_threshold_until_1d_is_subset_of_orginal();
	bool is_subset(cv::Mat image1D, cv::Mat image)
	{
		cv::Mat image_ = image.clone();
		
		int dilationSize = 1;
		cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                                       cv::Size( 2*dilationSize + 1, 2*dilationSize+1 ),
                                       cv::Point( dilationSize, dilationSize ) );
  		morphologyEx( image_, image_, cv::MORPH_DILATE, element, cv::Point(-1,-1), 1, cv::BORDER_REFLECT );

  		cv::bitwise_xor(image_, image1D, image_);
  		cv::bitwise_and(image_, image1D, image_);
  		int numberOfNonSubset = cv::countNonZero(image_);
  		
  		#ifdef WITH_TESTS
  		std::cout<<"process2d::is_subset::numberOfNonSubset: "<<numberOfNonSubset<<std::endl;
  		#endif

  		return !numberOfNonSubset;
	}

}