#ifndef PROCESS_2D_HPP
#define PROCESS_2D_HPP
#define MASK_PROCESS
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

class Process2D
{
	Process2D(cv::Mat image, cv::Mat image_1d){}
};
#endif