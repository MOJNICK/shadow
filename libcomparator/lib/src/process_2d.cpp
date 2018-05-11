#include "process_2d.hpp"
bool MaskIterateProcess::brighter()
{
	return (pix0[0] + pix0[1] + pix0[2]) > (pix1[0] + pix1[1] + pix1[2]);
}