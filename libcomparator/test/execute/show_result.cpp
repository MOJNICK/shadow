#include "show_result.hpp"

void save_result(const char* source_path, char const postFix[], char const outputFormat[], cv::Mat& image)
{
    char* destination_path = (char*)calloc( strlen(source_path) + strlen(postFix) + strlen(outputFormat) + strlen("\0"), sizeof(char) );
    
    const char* lastSlash = strrchr ( source_path, '.' );
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
        if(el1.get_clusterNumber() < el2.get_clusterNumber())
        {
            return true;
        }
        else
        {
            return false;
        }
    } );
    textPoint.erase( unique( textPoint.begin(), textPoint.end(), [](auto el1, auto el2){
        if(el1.get_clusterNumber() == el2.get_clusterNumber())
        {
            return true;
        }
        else
        {
            return false;
        }
    } ), textPoint.end() );
    std::for_each(textPoint.begin(), textPoint.end(), [&image](auto el){
        cv::putText(image, std::to_string(el.get_clusterNumber()), cv::Point(el.col, el.row ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.4, cv::Scalar(200,20,250));//, 1, 8, false);
        std::cout<<el.get_clusterNumber()<<" "<<el.row<<" "<<el.col<<"\n";
    });
    std::cout<< result.size()<< "\n";
    std::cout<< textPoint.size()<< "\n";
}

cv::Mat show_result(cv::Mat const img, std::vector<IndexTransitionCluster> const & result )
{
    cv::Mat image = img.clone();
    cv::namedWindow( "show_result", cv::WINDOW_AUTOSIZE );
    cv::imshow( "show_result", image );                   
    cv::waitKey(0);

    if(image.isContinuous())
    {
        for_each(result.begin(), result.end(), [&image](auto el){
            image.data[el.index( image )] = 255;
            image.data[el.index( image ) + 1] = 0; 
            image.data[el.index( image ) + 2] = 255;
        });
        draw_clusterNumber(image, result);
        cv::imshow( "show_result", image );
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
    cv::imshow( "show_result", blackImage );
    cv::waitKey(0);
    return blackImage;
}

std::vector<IndexTransitionCluster> test_on_image(const char* path, double factor, double eps, uint minPts)
{
    cv::Mat image;
    image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    if(! image.data )
    {
        std::cout<<"\nwrong path\n";
        return std::vector<IndexTransitionCluster>();
    }

    cv::resize(image, image, cv::Size(), factor, factor, cv::INTER_NEAREST);
    cv::Mat imageCpy = image.clone();
    cv::Mat imageCpy2 = image.clone();
    cv::Mat blackImage;

    TYPE acceptanceLevel = 60;
    ColorStruct entryBalance{ 1.0, 1.0, 1.0 };
    double lightThreshold = 0.2;
    double colorThreshold = 0.2;

    IterateProcess<TYPE> entryProcess(image, acceptanceLevel, lightThreshold, colorThreshold, entryBalance);
    auto result = entryProcess.iterate_HV();
    std::cout << result.size() << '\n';
    blackImage = show_result( image, std::vector<IndexTransitionCluster>( result.begin(), result.end() ) );
    save_result(path, "_noisy", ".png", blackImage);
    ColorBalance cba( image, 5u, 6 );
    ColorStruct secondBalance = cba.balance( result );

    blackImage = show_result( image, std::vector<IndexTransitionCluster>( result.begin(), result.end() ) );
    save_result(path, "_detect1", ".png", blackImage);
    result.resize( 0 );

    lightThreshold = 0.2;
    colorThreshold = 0.2;
    IterateProcess<TYPE> secondProcess(imageCpy2, acceptanceLevel, lightThreshold, colorThreshold, ColorStruct{ 0.82, 1.05, 1.14 });//secondBalance);
    result = secondProcess.iterate_HV();

    blackImage = show_result(imageCpy2, std::vector<IndexTransitionCluster>( result.begin(), result.end() ));
    save_result(path, "_detect_balanced", ".png", blackImage);
    
    Clustering clustering( result, Distance::distance_fast, eps, minPts);
    clustering.points_clustering(&Clustering::check_point_zone_linear);
    auto clusters = clustering.getRefVIndexTransitionCluster();
    blackImage = show_result(imageCpy, clusters);
    save_result(path, "_detect_cluster", ".png", blackImage);
    
    return clusters;
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

cv::Mat test_canny( char* path, double factor, int dilationSize )
{
    cv::Mat image;
    image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    if(! image.data )
    {
        std::cout<<"\nwrong path\n";
        return image;
    }
    cv::resize(image, image, cv::Size(), factor, factor, cv::INTER_NEAREST);

    Preprocess preprocess( MakeFilter::box_kernel(11), image);
    cv::Mat edges = preprocess.make_thick_kernel( image, dilationSize );
    cv::namedWindow( "Canny", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Canny", edges );
    cv::waitKey(0);

    return edges;
}

std::vector<IndexTransitionCluster> index_transition_part(cv::Mat const image, double eps, uint minPts, int compareDistance)
{
    cv::Mat imageCpy = image.clone();
    cv::Mat imageCpy2 = image.clone();
    cv::Mat blackImage;

    TYPE acceptanceLevel = 150;
    ColorStruct entryBalance{ 1.0, 1.0, 1.0 };
    double lightThreshold = 0.03;
    double colorThreshold = 0.01;

    IterateProcess<TYPE> entryProcess(image, acceptanceLevel, lightThreshold, colorThreshold, entryBalance, compareDistance);
    auto result = entryProcess.iterate_HV();
    
    ColorBalance cba( image, 5u, compareDistance );
    ColorStruct secondBalance = cba.balance( result );

    result.resize( 0 );
    //colorThreshold/=2;
    IterateProcess<TYPE> secondProcess(imageCpy2, acceptanceLevel, lightThreshold, colorThreshold, secondBalance, compareDistance);//secondBalance);
    result = secondProcess.iterate_HV();

    
    Clustering clustering( result, Distance::distance_fast, eps, minPts);
    clustering.points_clustering(&Clustering::check_point_zone_linear);
    auto clusters = clustering.getRefVIndexTransitionCluster();
    
    #ifdef VERBOSE    
    blackImage = show_result( image, std::vector<IndexTransitionCluster>( result.begin(), result.end() ) );
    blackImage = show_result(imageCpy2, std::vector<IndexTransitionCluster>( result.begin(), result.end() ));
    blackImage = show_result(imageCpy, clusters);
    #endif
    
    return clusters;
}

cv::Mat test_gauss_directed(const char* path, double factor, int dilationSize )
{
    cv::Mat image;
    image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    if(! image.data )
    {
        std::cout<<"\nwrong path\n";
        return image;
    }

    cv::Mat fullSize = image.clone();
    cv::resize(image, image, cv::Size(), factor, factor, cv::INTER_NEAREST);
    cv::Mat cImage = image.clone();
    bilateralFilter( cImage, image, 30, 150, 150, cv::BORDER_REFLECT );

    // int sigma = 2;
    // cv::GaussianBlur( image, image, cv::Size(sigma*4+1, sigma*4+1), sigma);

    auto idTrCluster = index_transition_part( image, 3.0, 1, 7 );
    std::vector<IndexTransition> idTr( idTrCluster.begin(), idTrCluster.end() );
/*
    Preprocess preprocess( MakeFilter::box_kernel(3), image);
    preprocess.make_thick_kernel(cImage, dilationSize);
    preprocess.rm_out_edge_detected( idTr );
*/

    #ifdef VERBOSE
    show_result( image, std::vector<IndexTransitionCluster>(idTr.begin(), idTr.end()));
    #endif

    Filter filter(cImage, idTr, 20, 5, 1, 10);
    cv::Mat result = filter.filter_image(fullSize);
/*
    ContourTransition contourTransition(image);
    contourTransition.bw_push_transition( idTr );
    cv::Mat matTrans = contourTransition.show_matDataTrans();
*/
    #ifdef VERBOSE
    cv::namedWindow( "matTrans", cv::WINDOW_AUTOSIZE );
    cv::imshow( "matTrans", matTrans );
    cv::waitKey(0);
    // cv::namedWindow( "thickKernel", cv::WINDOW_AUTOSIZE );
    // cv::imshow( "thickKernel", preprocess.get_thickKernel() );

    cv::namedWindow( "GaussFiltered", cv::WINDOW_AUTOSIZE );
    cv::imshow( "GaussFiltered", result );
    cv::waitKey(0);
    #endif

    return result;
}