#include "show_result.hpp"
#include "process_2d.hpp"
#include <typeinfo>

int main( int argc, char** argv )
{
    std::vector<std::string> vecArgv(argv, argv + argc);
    if(argc>3 && vecArgv[1]==std::string("--linearize"))
    {
        std::string inputFile = vecArgv[2];
        std::string outputFile = vecArgv[3];
        std::cout << "linearize_2_2_gamma: " << inputFile << " -> " << outputFile << '\n';
        cv::Mat img = cv::imread(inputFile, CV_LOAD_IMAGE_COLOR);
        if(!img.data){std::cout<<"cant open\n"; return 0;}
        linearize_2_2_gamma(img);
        cv::imwrite(outputFile, img);
    }
    else if(argc>3 && vecArgv[1]==std::string("--normalize"))
    {
        std::string inputFile = vecArgv[2];
        std::string outputFile = vecArgv[3];
        std::cout << "normalize: " << inputFile << " -> " << outputFile << '\n';
        cv::Mat img = cv::imread(inputFile, CV_LOAD_IMAGE_COLOR);
        if(!img.data){std::cout<<"cant open\n"; return 0;}
        cv::normalize(img, img, 0, 255, cv::NORM_MINMAX);
        cv::imwrite(outputFile, img);
    }
    else if(argc>4 && vecArgv[1]==std::string("--process2d"))
    {
        std::string inputFile = vecArgv[2];
        std::string inputFile1D = vecArgv[3];
        std::string outputFile = vecArgv[4];
        std::cout << "process2d: " << inputFile << " + " << inputFile1D << " -> " << outputFile << '\n';
        cv::Mat img   = cv::imread(inputFile, CV_LOAD_IMAGE_COLOR);
        cv::Mat img1D = cv::imread(inputFile1D, CV_LOAD_IMAGE_COLOR);
        if(!img.data || !img1D.data){std::cout<<"cant open\n"; return 0;}
        cv::Mat processed = process2d::run_process2d(img, img1D);

        cv::imwrite(outputFile, processed);
    }
    else if(argc>3 && vecArgv[1]==std::string("--iterate"))
    {
        std::string inputFile = vecArgv[2];
        std::string outputFile = vecArgv[3];
        std::cout << "iterate: " << inputFile << " -> " << outputFile << '\n';
        cv::Mat img = cv::imread(inputFile, CV_LOAD_IMAGE_COLOR);
        if(!img.data){std::cout<<"cant open\n"; return 0;}
        img = test_gauss_directed( inputFile.c_str(), 0.25, 1);
        cv::imwrite(outputFile, img);
    }
    else if(argc>3 && vecArgv[1]==std::string("--compare"))
    {
        std::string inputFile0 = vecArgv[2];
        std::string inputFile1 = vecArgv[3];
        std::cout << "compare: " << inputFile0 << " == " << inputFile1 << " -> ";
        cv::Mat img0   = cv::imread(inputFile0, CV_LOAD_IMAGE_COLOR);
        cv::Mat img1 = cv::imread(inputFile1, CV_LOAD_IMAGE_COLOR);
        if(!img0.data || !img1.data){std::cout<<"cant open\n"; return 0;}
        //bool eq = cv::countNonZero(img0!=img1) == 0;
        bool eq = (sum(img0 != img1) == cv::Scalar(0,0,0));
        std::cout << eq << "\n";
    }
    else if(argc>3 && vecArgv[1]==std::string("--difference"))
    {
        std::string inputFile0 = vecArgv[2];
        std::string inputFile1 = vecArgv[3];
        std::cout << "difference: " << inputFile0 << " == " << inputFile1 << " -> ";
        cv::Mat img0   = cv::imread(inputFile0, CV_LOAD_IMAGE_COLOR);
        cv::Mat img1 = cv::imread(inputFile1, CV_LOAD_IMAGE_COLOR);
        if(!img0.data || !img1.data){std::cout<<"cant open\n"; return 0;}
        double norm = norm( img0, img1, NORM_RELATIVE_L2);
        std::cout << norm << "\n";
    }
    return 0;
}
