#ifndef IMAGEPROCESS_HPP
	#define IMAGEPROCESS_HPP
	#include <opencv2/core/core.hpp>
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

#endif