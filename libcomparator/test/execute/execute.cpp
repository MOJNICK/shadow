#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "libcomparator.hpp"
#include "dataprocess.hpp"
#include <iostream>
#include "Python.h"


int main( int argc, char** argv )
{
    // PyObject *modname, *mod, *mdict, *func, *expr, *val1, *val2, *vars, *args, *rslt;           

    Py_Initialize();

    PyRun_SimpleString("import sys, os");
    PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname( os.getcwd()),'../shadow'))");
    // PyRun_SimpleString("import clustering");
    // modname = PyString_FromString("clustering");
    // mod = PyImport_Import(modname);

    size_t size = 12;
    int array = { 1,2,3,4,5,6,7,8,9,10,11,12 };
    PyObject *mymodule = PyImport_ImportModule( "clustering" );
    PyObject *myfunc = PyObject_GetAttrString( mymodule, "CApi.call_points_clustering" );
    PyObject *mylist = PyList_New(size);
    for (size_t i = 0; i != size; ++i) {
        PyList_SET_ITEM(mylist, i, PyInt_FromLong(array[i]));
    }
    PyObject *arglist = Py_BuildValue("(o)", mylist);
    PyObject *result = PyObject_CallObject(myfunc, arglist);
    int retval = (int)PyInt_AsLong(result);
    Py_DECREF(result);
    Py_DECREF(arglist);
    Py_DECREF(mylist);
    Py_DECREF(myfunc);
    Py_DECREF(mymodule);
    return retval;

    Py_Finalize();

	cv::Mat image;
    image = cv::imread("/home/szozda/Downloads/reference_image2.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file "/Downloads/reference_image.jpg"


    if(! image.data )                              // Check for invalid input
    {
    	std::cout<<"wrong\n";
        return -1;
    }

    double factor = 0.25;
    cv::resize(image, image, cv::Size(), factor, factor);

    TYPE acceptanceLevel = 50;
    double balance[] = {1.0, 1.0, 1.0};
    double lightThreshold = 0.1;
    double colorThreshold = 0.001;
    IterateProcess<TYPE> iterateProcess(image, acceptanceLevel, lightThreshold, colorThreshold, balance);
    auto result = iterateProcess.iterate_HV();
    DataProcess::concatenate_HV(result);
  
    if(image.isContinuous())
    {
    	for_each(result.begin(), result.end(), [&image](auto el){
    		image.data[el.index] = 255;
    		image.data[el.index + 1] = 0; 
    		image.data[el.index + 2] = 255;
    	});
    	
	    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
	    cv::imshow( "Display window", image );                   // Show our image inside it.
	}
	else
	{
		std::cout<<"notcontionuous\n";
	}
    cv::waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
