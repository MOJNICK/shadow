#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "libcomparator.hpp"
#include "dataprocess.hpp"
#include <iostream>


int main( int argc, char** argv )
{
	cv::Mat image;
    image = cv::imread("/home/szozda/Downloads/reference_image2.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file "/Downloads/reference_image.jpg"


    if(! image.data )                              // Check for invalid input
    {
    	std::cout<<"\nwrong path\n";
        return -1;
    }

    double factor = 1;
    cv::resize(image, image, cv::Size(), factor, factor, cv::INTER_NEAREST);

    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", image );                   // Show our image inside it.
    cv::waitKey(0);                                          // Wait for a keystroke in the window

    TYPE acceptanceLevel = 250;
    double balance[] = {1.0, 1.0, 1.0};
    double lightThreshold = 0.3;
    double colorThreshold = 0.01;
    IterateProcess<TYPE> iterateProcess(image, acceptanceLevel, lightThreshold, colorThreshold, balance);
    auto result = iterateProcess.iterate_HV();
    DataProcess::concatenate_HV(result);
  
    if(image.isContinuous())
    {
    	for_each(result.begin(), result.end(), [&image](auto el){
    		image.data[el.index( image )] = 255;
    		image.data[el.index( image ) + 1] = 0; 
    		image.data[el.index( image ) + 2] = 255;
    	});
    	
	    cv::imshow( "Display window", image );                   // Show our image inside it.
	}
	else
	{
		std::cout<<"notcontionuous\n";
	}
    cv::waitKey(0);                                          // Wait for a keystroke in the window

    cv::Mat blackImage(image.rows, image.cols, CV_8UC3, cv::Scalar(0,0,0));
    for_each(result.begin(), result.end(), [&blackImage](auto el){
        blackImage.data[el.index( blackImage )] = 255;
        blackImage.data[el.index( blackImage ) + 1] = 0; 
        blackImage.data[el.index( blackImage ) + 2] = 255;
    });
    
    cv::imshow( "Display window", blackImage );                   // Show our image inside it.
    cv::waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
