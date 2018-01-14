#include "show_result.hpp"

void save_result(char* source_path, char const postFix[], char const outputFormat[], cv::Mat& image)
{
    char* destination_path = (char*)calloc( strlen(source_path) + strlen(postFix) + strlen(outputFormat), sizeof(char) );

    char* lastSlash = strrchr ( source_path, '.' );
    int lastSlashPos = lastSlash - source_path;
    
    strncpy( destination_path, source_path, lastSlashPos );
    
    int len = strlen( destination_path );
    strcpy( destination_path + len, postFix );
    
    len += strlen( postFix );
    strcpy( destination_path + len, source_path + lastSlashPos );
    
    len = strlen( destination_path );
    strcpy( destination_path + len, outputFormat );

    cv::imwrite( destination_path, image);
}

void p_around(unsigned char* data, int before, int after)
{
    for(int i = -before; i < after; ++i )
    {
        if(i == 0)
        {
            std::cout << "MARKER";
        }
        std::cout << data[i];
    }
}

void draw_clusterNumber(cv::Mat& image, std::vector<IndexTransitionCluster> const & result)
{
    std::vector<IndexTransitionCluster> textPoint(result);
    std::stable_sort( textPoint.begin(), textPoint.end(), [](auto el1, auto el2){
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
        cv::putText(image, std::to_string(el.getClusterNumber()), cv::Point(el.col, el.row ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.4, cv::Scalar(200,20,250));//, 1, 8, false);
        std::cout<<el.getClusterNumber()<<" "<<el.row<<" "<<el.col<<"\n";
    });
    std::cout<< result.size()<< "\n";
    std::cout<< textPoint.size()<< "\n";
}

cv::Mat show_result(cv::Mat img, std::vector<IndexTransitionCluster> const & result )
{
    cv::Mat image = img.clone();
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
        return image;
    }
//    cv::waitKey(0);

    cv::Mat blackImage(image.rows, image.cols, CV_8UC3, cv::Scalar(0,0,0));
    for_each(result.begin(), result.end(), [&blackImage](auto el){
        blackImage.data[el.index( blackImage )] = 255;
        blackImage.data[el.index( blackImage ) + 1] = 0; 
        blackImage.data[el.index( blackImage ) + 2] = 255;
    });
    draw_clusterNumber(blackImage, result);
    cv::imshow( "Display window", blackImage );
    cv::waitKey(0);
    return blackImage;
}

int test_on_image(char path[], double factor, double eps, uint minPts)
{
    cv::Mat image;
    image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    if(! image.data )
    {
        std::cout<<"\nwrong path\n";
        return -1;
    }

    cv::resize(image, image, cv::Size(), factor, factor, cv::INTER_NEAREST);
    cv::Mat imageCpy = image.clone();
    cv::Mat imageCpy2 = image.clone();
    cv::Mat blackImage;

    TYPE acceptanceLevel = 90;
    ColorStruct entryBalance{ 1.0, 1.0, 1.0 };
    double lightThreshold = 0.3;
    double colorThreshold = 0.2;
    IterateProcess<TYPE> entryProcess(image, acceptanceLevel, lightThreshold, colorThreshold, entryBalance);
    auto result = entryProcess.iterate_HV();
    std::cout << result.size() << '\n';
    DataProcess::concatenate_HV(result);
    blackImage = show_result( image, std::vector<IndexTransitionCluster>( result.begin(), result.end() ) );
    save_result(path, "_noisy", ".png", blackImage);
    DataProcess::remove_noise_matches(result);
    ColorBalance cba( image, 5u, 6 );
    ColorStruct secondBalance = cba.balance( result );

    blackImage = show_result( image, std::vector<IndexTransitionCluster>( result.begin(), result.end() ) );
    save_result(path, "_detect1", ".png", blackImage);
    result.resize( 0 );

    lightThreshold = 0.3;
    colorThreshold = 0.2;
    IterateProcess<TYPE> secondProcess(imageCpy2, acceptanceLevel, lightThreshold, colorThreshold, ColorStruct{ 0.82, 1.05, 1.14 });//secondBalance);
    result = secondProcess.iterate_HV();
    DataProcess::concatenate_HV(result);
    DataProcess::remove_noise_matches(result);

    blackImage = show_result(imageCpy2, std::vector<IndexTransitionCluster>( result.begin(), result.end() ));
    save_result(path, "_detect_balanced", ".png", blackImage);
    
    Clustering clustering( result, Distance::distance_fast, eps, minPts);
    clustering.points_clustering(&Clustering::check_point_zone_linear);
    auto clusters = clustering.getRefVIndexTransitionCluster();
    blackImage = show_result(imageCpy, clusters);
    save_result(path, "_detect_cluster", ".png", blackImage);
    
    return 0;
}


int broad_HUE(char* path)
{
    cv::Mat image;
    image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    if(! image.data )
    {
        std::cout<<"\nwrong path\n";
        return -1;
    }
    
    cvtColor(image, image, CV_BGR2HSV);
    for(int i =2; i < image.rows * image.cols * 3; i+=3)
    {
        image.data[i] = 255;
    }
    cvtColor(image, image, CV_HSV2BGR);
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display window", image );
    save_result(path, "_OUT1", ".png", image);


    cv::Mat cpyImage = image.clone();
    cvtColor(cpyImage, cpyImage, CV_BGR2HSV);
    for(int i =2; i < cpyImage.rows * cpyImage.cols * 3; i+=3)
    {
        cpyImage.data[i] = cpyImage.data[i-1];
    }
    cvtColor(cpyImage, cpyImage, CV_HSV2BGR);
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display window", cpyImage );
    save_result(path, "_OUT3", ".png", cpyImage);


    cvtColor(image, image, CV_BGR2HSV);
    for(int i =2; i < image.rows * image.cols * 3; i+=3)
    {
        image.data[i] = 255;
    }
    for(int i = 1; i < image.rows * image.cols * 3; i+=3)
    {
        image.data[i] = 255;
    }
    cvtColor(image, image, CV_HSV2BGR);
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display window", image );
    save_result(path, "_OUT2", ".png", image);

    return 0;
}