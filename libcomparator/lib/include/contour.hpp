#ifndef CONTOUR_HPP
#define CONTOUR_HPP
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "libcomparator.hpp"
#include "dataprocess.hpp"
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
	void bw_push_transition(std::vector<IndexTransition> const & indexTransition);
	cv::Mat show_matDataTrans();
private:
	void copy_data( cv::Mat& image );
	void set_transition_to_no();
};

class Preprocess
{
public:
	Preprocess( cv::Mat_<double> filterKernel_, cv::Mat const & image );

	cv::Mat& get_thickKernel(){ return thickKernel; };

	void rm_out_edge_detected( std::vector<IndexTransition> & indexTransition );

	cv::Mat make_thick_kernel( cv::Mat const & image, uint dilationSize );
	ContourTransition get_correction_edge( cv::Mat const & image, std::vector<IndexTransition> const & indexTransition, uint dilationSize );
private:
	cv::Mat_<double> filterKernel;
	cv::Mat thickKernel;
	cv::Size2i srcImgSize;

	Transition get_direction( int const row, int const col, cv::Mat_<Transition> matTrans );
	// DataTransition set_direction( cv::Mat thickKernelROI, cv::Mat_<Transition> matTrans );
	cv::Mat_<Transition> cvt_it_to_matTSilent( std::vector<IndexTransition> const & indexTransition );
};




class MakeFilter
{
	enum class KernelType
	{
		gauss,
		triangle
	}kernelType;
public:
	MakeFilter(KernelType kernelType = KernelType::gauss) : kernelType{kernelType}{}

	static cv::Mat box_kernel( int height, int width = -1 );
	cv::Mat operator()( double sigma, double sizeFactor, Transition direction, double hvFactor ){return calc_antisimmetric_filter( sigma, sizeFactor, direction, hvFactor );}
private:
	static void cvt_to_antisimmetric( cv::Mat& kernel, Transition direction, int anchorH = -1, int anchorV = -1);
	cv::Mat calc_antisimmetric_filter( double sigma, double sizeFactor, Transition direction, double hvFactor );
	static cv::Mat gauss_kernel( cv::Size kernelSigma, cv::Size kernelSize );
	static cv::Mat triangle_kernel( cv::Size kernelSigma, cv::Size kernelSize = cv::Size(-1, -1) );
};

class Filter
{
	enum class FilterMode
	{
		normal,
		grabCut
	}filterMode;
public:
	Filter( cv::Mat & image, std::vector<IndexTransition> const & indexTransition,
			double sizeFactor = 10, double antiSigma = 5, double hvFactor = 1,
			uint calcDistance = 1, FilterMode filterMode = FilterMode::grabCut );
	
	cv::Mat filter_image(cv::Mat image);

private:
	cv::Mat & _image;
	cv::Mat reverseZeroBasedFilter;
	cv::Size2i srcImgSize;

	double sizeFactor;
	double antiSigma;
	double hvFactor;
	cv::Vec3d correctionPower;

	cv::Vec3d calc_correction_power( std::vector<IndexTransition> const & indexTransition, uint calcDistance );
	void get_shadow_weight( std::vector<IndexTransition> const & indexTransition );
	cv::Mat cvt_it_to_matFloat( std::vector<IndexTransition> const & indexTransition );

};
#endif