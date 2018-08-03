#include "show_result.hpp"
#include "process_2d.hpp"
#include <typeinfo>

namespace debug_norm{
    using namespace cv;
template<typename _Tp> static double
norm_(const _Tp* src, size_t total, int cn, int normType, double startval, const uchar* mask)
{
    size_t i;
    double result = startval;
    if( !mask )
        total *= cn;

    if( normType == cv::NORM_INF )
    {
        if( !mask )
            for( i = 0; i < total; i++ )
                result = std::max(result, (double)std::abs(0+src[i]));// trick with 0 used to quiet gcc warning
        else
            for( int c = 0; c < cn; c++ )
            {
                for( i = 0; i < total; i++ )
                    if( mask[i] )
                        result = std::max(result, (double)std::abs(0+src[i*cn + c]));
            }
    }
    else if( normType == cv::NORM_L1 )
    {
        if( !mask )
            for( i = 0; i < total; i++ )
                result += std::abs(0+src[i]);
        else
            for( int c = 0; c < cn; c++ )
            {
                for( i = 0; i < total; i++ )
                    if( mask[i] )
                        result += std::abs(0+src[i*cn + c]);
            }
    }
    else
    {
        if( !mask )
            for( i = 0; i < total; i++ )
            {
                double v = src[i];
                result += v*v;
            }
        else
            for( int c = 0; c < cn; c++ )
            {
                for( i = 0; i < total; i++ )
                    if( mask[i] )
                    {
                        double v = src[i*cn + c];
                        result += v*v;
                    }
            }
    }
    return result;
}


template<typename _Tp> static double
norm_(const _Tp* src1, const _Tp* src2, size_t total, int cn, int normType, double startval, const uchar* mask)
{
    size_t i;
    double result = startval;
    if( !mask )
        total *= cn;

    if( normType == cv::NORM_INF )
    {
        if( !mask )
            for( i = 0; i < total; i++ )
                result = std::max(result, (double)std::abs(src1[i] - src2[i]));
        else
            for( int c = 0; c < cn; c++ )
            {
                for( i = 0; i < total; i++ )
                    if( mask[i] )
                        result = std::max(result, (double)std::abs(src1[i*cn + c] - src2[i*cn + c]));
            }
    }
    else if( normType == cv::NORM_L1 )
    {
        if( !mask )
            for( i = 0; i < total; i++ )
                result += std::abs(src1[i] - src2[i]);
        else
            for( int c = 0; c < cn; c++ )
            {
                for( i = 0; i < total; i++ )
                    if( mask[i] )
                        result += std::abs(src1[i*cn + c] - src2[i*cn + c]);
            }
    }
    else
    {
        if( !mask )
            for( i = 0; i < total; i++ )
            {
                double v = src1[i] - src2[i];
                result += v*v;
            }
        else
            for( int c = 0; c < cn; c++ )
            {
                for( i = 0; i < total; i++ )
                    if( mask[i] )
                    {
                        double v = src1[i*cn + c] - src2[i*cn + c];
                        result += v*v;
                    }
            }
    }
    return result;
}


double norm(const Mat& src, int normType, const Mat& mask=Mat())
{
    int normType0 = normType;
    normType = normType == cv::NORM_L2SQR ? cv::NORM_L2 : normType;

    CV_Assert( mask.empty() || (src.size == mask.size && mask.type() == CV_8U) );
    CV_Assert( normType == cv::NORM_INF || normType == cv::NORM_L1 || normType == cv::NORM_L2 );

    const Mat *arrays[]={&src, &mask, 0};
    Mat planes[2];

    NAryMatIterator it(arrays, planes);
    size_t total = planes[0].total();
    size_t i, nplanes = it.nplanes;
    int depth = src.depth(), cn = planes[0].channels();
    double result = 0;

    for( i = 0; i < nplanes; i++, ++it )
    {
        const uchar* sptr = planes[0].data;
        const uchar* mptr = planes[1].data;

        switch( depth )
        {
        case CV_8U:
            result = norm_((const uchar*)sptr, total, cn, normType, result, mptr);
            break;
        case CV_8S:
            result = norm_((const schar*)sptr, total, cn, normType, result, mptr);
            break;
        case CV_16U:
            result = norm_((const ushort*)sptr, total, cn, normType, result, mptr);
            break;
        case CV_16S:
            result = norm_((const short*)sptr, total, cn, normType, result, mptr);
            break;
        case CV_32S:
            result = norm_((const int*)sptr, total, cn, normType, result, mptr);
            break;
        case CV_32F:
            result = norm_((const float*)sptr, total, cn, normType, result, mptr);
            break;
        case CV_64F:
            result = norm_((const double*)sptr, total, cn, normType, result, mptr);
            break;
        default:
            CV_Error(CV_StsUnsupportedFormat, "");
        };
    }
    if( normType0 == NORM_L2 )
        result = sqrt(result);
    return result;
}


double norm(const Mat& src1, const Mat& src2, int normType, const Mat& mask=Mat())
{
    int normType0 = normType;
    normType = normType == NORM_L2SQR ? NORM_L2 : normType;

    CV_Assert( src1.type() == src2.type() && src1.size == src2.size );
    CV_Assert( mask.empty() || (src1.size == mask.size && mask.type() == CV_8U) );
    CV_Assert( normType == NORM_INF || normType == NORM_L1 || normType == NORM_L2 );
    const Mat *arrays[]={&src1, &src2, &mask, 0};
    Mat planes[3];

    NAryMatIterator it(arrays, planes);
    size_t total = planes[0].total();
    size_t i, nplanes = it.nplanes;
    int depth = src1.depth(), cn = planes[0].channels();
    double result = 0;

    for( i = 0; i < nplanes; i++, ++it )
    {
        const uchar* sptr1 = planes[0].data;
        const uchar* sptr2 = planes[1].data;
        const uchar* mptr = planes[2].data;

        switch( depth )
        {
        case CV_8U:
            result = norm_((const uchar*)sptr1, (const uchar*)sptr2, total, cn, normType, result, mptr);
            break;
        case CV_8S:
            result = norm_((const schar*)sptr1, (const schar*)sptr2, total, cn, normType, result, mptr);
            break;
        case CV_16U:
            result = norm_((const ushort*)sptr1, (const ushort*)sptr2, total, cn, normType, result, mptr);
            break;
        case CV_16S:
            result = norm_((const short*)sptr1, (const short*)sptr2, total, cn, normType, result, mptr);
            break;
        case CV_32S:
            result = norm_((const int*)sptr1, (const int*)sptr2, total, cn, normType, result, mptr);
            break;
        case CV_32F:
            result = norm_((const float*)sptr1, (const float*)sptr2, total, cn, normType, result, mptr);
            break;
        case CV_64F:
            result = norm_((const double*)sptr1, (const double*)sptr2, total, cn, normType, result, mptr);
            break;
        default:
            CV_Error(CV_StsUnsupportedFormat, "");
        };
    }
    if( normType0 == NORM_L2 )
        result = sqrt(result);
    return result;
}
}

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
    else if(argc>3 && vecArgv[1]==std::string("--relative_norm"))
    {
        std::string inputFile0 = vecArgv[2];
        std::string inputFile1 = vecArgv[3];
        std::cout << "relative_norm: " << inputFile0 << " == " << inputFile1 << " -> ";
        cv::Mat img0   = cv::imread(inputFile0, CV_LOAD_IMAGE_COLOR);
        cv::Mat img1 = cv::imread(inputFile1, CV_LOAD_IMAGE_COLOR);
        if(!img0.data || !img1.data){std::cout<<"cant open\n"; return 0;}
        cv::resize(img0,img0,img1.size());
        double norm = 1.0 - debug_norm::norm( img0, img1, cv::NORM_L2) / debug_norm::norm(img1, cv::NORM_L2);
        std::cout << norm << '\n';
    }
    return 0;
}
