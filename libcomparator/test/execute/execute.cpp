#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "libcomparator.hpp"
#include "dataprocess.hpp"
#include "clustering.hpp"
#include <iostream>

void p_around(unsigned char* data, int before, int after)
{
    for(int i = -before; i < after; ++i )
    {
        if(i == 0)
        {
            std::cout << "MARKER";
        }
        std::cout<<data[i];
    }
}

void draw_clusterNumber(cv::Mat& image, std::vector<IndexTransitionCluster> const & result)
{
    std::vector<IndexTransitionCluster> textPoint(result);
    sort( textPoint.begin(), textPoint.end(), [](auto el1, auto el2){
        if(el1.getClusterNumber() < el2.getClusterNumber())
        {
            return true;
        }
        else
        {
            return false;
        }
    } );
    textPoint.erase( unique( textPoint.begin(), textPoint.end(), [](auto el1, auto el2){
        if(el1.getClusterNumber() == el2.getClusterNumber())
        {
            return true;
        }
        else
        {
            return false;
        }
    } ), textPoint.end() );
    std::for_each(textPoint.begin(), textPoint.end(), [&image](auto el){
        cv::putText(image, std::to_string(el.getClusterNumber()), cv::Point(el.col / 3, el.row ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,20,250), 1, 8, false);
        std::cout<<el.getClusterNumber()<<" "<<el.row<<" "<<el.col<<"\n";
    });
    std::cout<< result.size()<< "\n";
    std::cout<< textPoint.size()<< "\n";
}

void show_result(cv::Mat image, std::vector<IndexTransitionCluster> const & result)
{
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display window", image );                   
    cv::waitKey(0);

    if(image.isContinuous())
    {
        for_each(result.begin(), result.end(), [&image](auto el){
            image.data[el.index( image )] = 255;
            image.data[el.index( image ) + 1] = 0; 
            image.data[el.index( image ) + 2] = 255;
        });
        draw_clusterNumber(image, result);
        cv::imshow( "Display window", image );
    }
    else
    {
        std::cout<<"notcontionuous\n";
        return;
    }
    cv::waitKey(0);

    cv::Mat blackImage(image.rows, image.cols, CV_8UC3, cv::Scalar(0,0,0));
    for_each(result.begin(), result.end(), [&blackImage](auto el){
        blackImage.data[el.index( blackImage )] = 255;
        blackImage.data[el.index( blackImage ) + 1] = 0; 
        blackImage.data[el.index( blackImage ) + 2] = 255;
    });
    draw_clusterNumber(blackImage, result);
    cv::imshow( "Display window", blackImage );
    cv::waitKey(0);
}

int test_on_image(char const path[], double eps, uint minPts)
{
    cv::Mat image;
    image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    if(! image.data )
    {
        std::cout<<"\nwrong path\n";
        return -1;
    }
    cv::Mat imageCpy = image.clone();


    double factor = 1;
    cv::resize(image, image, cv::Size(), factor, factor, cv::INTER_NEAREST);

    TYPE acceptanceLevel = 250;
    double balance[] = {1.0, 1.0, 1.0};
    double lightThreshold = 0.2;
    double colorThreshold = 100000;
    IterateProcess<TYPE> iterateProcess(image, acceptanceLevel, lightThreshold, colorThreshold, balance);
    auto result = iterateProcess.iterate_HV();
    DataProcess::concatenate_HV(result);
    
    show_result(image, std::vector<IndexTransitionCluster>( result.begin(), result.end() ));
    
    Clustering clustering( result, Distance::distance_fast, eps, minPts);
    clustering.points_clustering(&Clustering::check_point_zone_linear);
    auto clusters = clustering.getRefVIndexTransitionCluster();
    show_result(imageCpy, clusters);
    
    return 0;
}

int main( int argc, char** argv )
{
    test_on_image("/home/szozda/Downloads/refImg/circTst.png", 3.0, 5);

    return 0;
}
