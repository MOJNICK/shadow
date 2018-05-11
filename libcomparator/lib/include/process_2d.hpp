#ifndef PROCESS_2D_HPP
#define PROCESS_2D_HPP
#include "libcomparator.hpp"

class MaskIterateProcess
{
public:
	std::vector<IndexTransition> calc_transitions_with_mask(cv::Mat const & image_1d, cv::Mat const & mask);
private:
	unsigned char* pix0;
	unsigned char* pix1;
	Transition calc_transition();
	bool brighter();
};
#endif