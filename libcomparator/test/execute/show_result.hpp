#ifndef SHOW_RESULT
#define SHOW_RESULT

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "libcomparator.hpp"
#include "dataprocess.hpp"
#include "clustering.hpp"
#include "contour.hpp"
#include <iostream>

void save_result(char* source_path, char const postFix[], char const outputFormat[], cv::Mat& image);

void p_around(unsigned char* data, int before, int after);

void draw_clusterNumber(cv::Mat& image, std::vector<IndexTransitionCluster> const & result);

cv::Mat show_result(cv::Mat img, std::vector<IndexTransitionCluster> const & result );

std::vector<IndexTransitionCluster> test_on_image(char path[], double factor, double eps, uint minPts);

int broad_HUE(char* path);

cv::Mat test_canny( char* path, double factor, int dilationSize );

cv::Mat test_gauss_directed( char* path, double factor, int dilationSize );

#endif