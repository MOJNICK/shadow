#include "show_result.hpp"
#include <iostream>


int main( int argc, char** argv )
{
    test_on_image("/home/szozda/Downloads/refImg/girRef.jpg", 0.25, 3.0, 10);
    // test_on_image("/home/szozda/Downloads/refImg/linThin.png", 1, 6.0, 2);
    test_on_image("/home/szozda/Downloads/refImg/linThick.png", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/appRef.jpg", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/roof.png", 0.25, 6.0, 2); 
    // test_on_image("/home/szozda/Downloads/refImg/cirRef.png", 1, 2.0, 100);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef2.png", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef3.png", 1, 6.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef4.png", 1, 2.0, 2);
    // test_on_image("/home/szozda/Downloads/refImg/cirRef5.png", 1, 6.0, 2);


   broad_HUE("/home/szozda/Downloads/refImg/girRef.jpg");
//    broad_HUE("/home/szozda/Downloads/refImg/palma.jpg");
//    broad_HUE("/home/szozda/Downloads/refImg/table.jpg");
//    broad_HUE("/home/szozda/Downloads/refImg/palma_linear.png");
//    broad_HUE("/home/szozda/Downloads/refImg/roof_black.png");
//    broad_HUE("/home/szozda/Downloads/refImg/roof_invariant.png");
//    broad_HUE("/home/szozda/Downloads/refImg/roof_finlay_invariant.png");

    return 0;
}
