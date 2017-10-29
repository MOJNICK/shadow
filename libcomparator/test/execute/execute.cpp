#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "libcomparator.hpp"
#include <iostream>

int main( int argc, char** argv )
{
	cv::Mat image;
    image = cv::imread("~/Downloads/reference_image.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
    	std::cout<<"wrong";
        return -1;
    }

    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", image );                   // Show our image inside it.

    cv::waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
