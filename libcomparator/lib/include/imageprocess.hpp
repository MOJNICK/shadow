#ifndef IMAGEPROCESS_HPP
	#define IMAGEPROCESS_HPP
	#include <opencv2/core/core.hpp>
	#include <opencv2/imgproc/imgproc.hpp>
	#include <opencv2/highgui/highgui.hpp>
	
	inline void change_gamma( cv::Mat& img, double actual_gamma, double target_gamma = 1.0)
	{
		double inverse_gamma = target_gamma / actual_gamma;
		
		cv::Mat lut_matrix(1, 256, CV_8UC1 );
		uchar * ptr = lut_matrix.ptr();
		for( int i = 0; i < 256; i++ )
		  ptr[i] = (int)( pow( (double) i / 255.0, inverse_gamma ) * 255.0 );
		
		LUT( img, lut_matrix, img );
	}

	inline void linearize_2_2_gamma(cv::Mat& img)
	{
		change_gamma(img, 2.2);
	}

	inline cv::Mat present_grabCut_mask(cv::Mat grabCutOutput, bool imshow = true)
	{
		cv::Mat mask = grabCutOutput.clone();
		cv::Mat lut(1, 256, CV_8U);
		lut.data[cv::GC_BGD] = 255;
		lut.data[cv::GC_PR_BGD] = 150;
		lut.data[cv::GC_PR_FGD] = 100;
		lut.data[cv::GC_FGD] = 0;
		cv::LUT(mask, lut, mask);
		if(imshow)
		{
			cv::imshow("present_grabCut_mask", mask);
			cv::waitKey(0);
		}
		return mask;
	}
#endif