#ifndef PROCESS_2D_HPP
#define PROCESS_2D_HPP
#define MASK_PROCESS
#include "libcomparator.hpp"

double balance[] = {1.0, 1.0, 1.0};

class MaskIterateProcess: public IterateProcessMask<TYPE>
{
public:
	MaskIterateProcess(cv::Mat img, cv::Mat mask)
	:
		IterateProcessMask<TYPE>
		(
			img,
			std::numeric_limits<TYPE>::lowest(),
			std::numeric_limits<double>::min(),
			std::numeric_limits<double>::max(),
			balance,
			mask
		)
	{}
	std::vector<IndexTransition> calc_transitions_with_mask(cv::Mat const & image_1d, cv::Mat const & mask);
private:
	unsigned char* pix0;
	unsigned char* pix1;
	Transition calc_transition();
};
#endif