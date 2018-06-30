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
        img = test_gauss_directed( inputFile.c_str(), 1, 1);
        cv::imwrite(outputFile, img);
    }
    return 0;

    test_canny( "/home/szozda/Downloads/refImg/girRef.jpg", 0.25, 0 );

    // cv::Mat image;
    // image = cv::imread("/home/szozda/Downloads/refImg/girRef.jpg", CV_LOAD_IMAGE_COLOR);
    // if(! image.data )
    // {
    //     std::cout<<"\nwrong path\n";
    //     return -1;
    // }
    // cv::GaussianBlur( image, image, cv::Size(51,51), 30 , 1 );
    // // cv::copyMakeBorder( image, image, 100, 100, 300, 300, cv::BORDER_REFLECT);//to be safe
    // cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    // cv::imshow( "Display window", image );
    // std::cout<<image.total()<<"\n";
    // cv::waitKey(0);

    // cv::Rect rect(10, 100, 30, 800);
    // cv::Mat roi = image(rect);
    // cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    // cv::imshow( "Display window", roi );
    // std::cout<<roi.total()<<"\n";
    // cv::waitKey(0);

    // std::cout<<std::endl;
    // std::cout<<std::endl;

    // cv::Mat testN(7, 6, CV_8UC1 );
    // for( int i = 0; i<testN.total(); ++i )
    // {
    //     testN.data[i] = i;
    // }

    // cv::Rect rct(2, 1, 3, 2);

    // cv::Mat testR = testN(rct);
    // for( int i = 0; i < testN.total(); ++i )
    // {
    //     // std::cout << (int)( *testN.ptr( i ) ) << " ";
    //     std::cout << (int)( testN.data[ i ] ) << " ";
    // }
    // std::cout<<std::endl;
    // for( int i = 0; i < testR.total(); ++i )
    // {
    //     // std::cout << (int)( *testR.ptr( i ) ) << " ";
    //     std::cout << (int)( testR.data[ i ] ) << " ";
    // }
    // std::cout<<std::endl;
    // std::cout<<std::endl;


    // for( int i = 0; i < testN.rows; ++i )
    //     for( int j = 0; j < testN.cols; ++j )
    //     {
    //         std::cout << (int)( *testN.ptr( i, j ) ) << " ";
    //     }
    // std::cout<<std::endl;
    // for( int i = 0; i < testR.rows; ++i )
    //     for( int j = 0; j < testR.cols; ++j )
    //     {
    //         std::cout << (int)( *testR.ptr( i, j ) ) << " ";
    //     }
    // std::cout<<std::endl;
    // std::cout<<std::endl;

    // test_directed_canny("/home/szozda/Downloads/refImg/girRef.jpg", 0.25, 3);

    // std::cout<<std::endl;
    // std::cout<<std::endl;
    // std::cout<< (static_cast<int>(Transition::all) >> shiftToDistinct);
    // std::cout<<std::endl;
    // std::cout<<std::endl;
    


    test_gauss_directed( "/home/szozda/Downloads/refImg/girRef.jpg", 0.25, 1);

    // test_on_image("/home/szozda/Downloads/refImg/girRef.jpg", 0.25, 3.0, 10);
    // test_on_image("/home/szozda/Downloads/refImg/linThin.png", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/linThick.png", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/appRef.jpg", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/roof.png", 0.25, 6.0, 2); 
    // test_on_image("/home/szozda/Downloads/refImg/cirRef.png", 1, 2.0, 100);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef2.png", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef3.png", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef4.png", 1, 2.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef5.png", 1, 6.0, 2);


   // broad_HUE("/home/szozda/Downloads/refImg/girRef.jpg");
//    broad_HUE("/home/szozda/Downloads/refImg/palma.jpg");
//    broad_HUE("/home/szozda/Downloads/refImg/table.jpg");
//    broad_HUE("/home/szozda/Downloads/refImg/palma_linear.png");
//    broad_HUE("/home/szozda/Downloads/refImg/roof_black.png");
//    broad_HUE("/home/szozda/Downloads/refImg/roof_invariant.png");
//    broad_HUE("/home/szozda/Downloads/refImg/roof_finlay_invariant.png");

    return 0;
}
