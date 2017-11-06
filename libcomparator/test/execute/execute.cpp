#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "libcomparator.hpp"
#include "dataprocess.hpp"
#include <iostream>
#include "Python.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
// #include "ndarrayobject.h"
#include "numpy/arrayobject.h"

int main( int argc, char** argv )
{
    {
        Py_Initialize();
        
        PyObject* osmodule = PyImport_ImportModule("os");
        PyObject* sysmodule = PyImport_ImportModule("sys");        
        
        PyObject* syspath = PyObject_GetAttrString( sysmodule, "path" );
        // PyObject_Print( syspath, stdout, 0 );
        PyObject* ospath = PyObject_GetAttrString( osmodule, "path" );
        PyObject* osgetcwd = PyObject_GetAttrString( osmodule, "getcwd" );
        PyObject* osgetcwdresult = PyObject_CallObject( osgetcwd, NULL );
        
        PyObject* shadowstring = PyString_FromString("/../shadow");
        PyObject* ospathdirnameresult = PyObject_CallMethod (ospath, (char *)"dirname", (char *)"O", osgetcwdresult );


        
        // PyObject* tuplearg = Py_BuildValue("OO", ospathdirnameresult, shadowstring);
        // PyObject* ospathjoinresult = PyObject_CallMethod( ospath, (char *)"join", (char *)"OO", ospathdirnameresult, shadowstring );
        PyObject* list = PyList_New(0);
        PyList_Append( list, ospathdirnameresult);
        PyList_Append( list, shadowstring );
        PyObject* emptystring = PyString_FromString("");
        PyObject* ospathjoinresult = PyObject_CallMethod( emptystring, (char *)"join", (char *)"O", list );
        
        PyList_Append( syspath, ospathjoinresult );
        
        

        // PyObject* dirnamestring = PyString_FromString("dirname");
        // PyObject* ospathdirnameresult PyObject_CallMethodObjArgs(ospath, dirnamestring, ..., NULL)


        // PyObject* ospathdirnameresult = PyObject_CallMethod(ospath, "dirname", "O", osgetcwdresult, shadowstring);
        // PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname( os.getcwd()),'../shadow'))");
        // PyObject* osgetcwdresult = PyObject_CallMethod(osmodule, "getcwd", NULL);
        // char* PyString_AsString(PyObject *string)
        // PyObject* modname = PyString_FromString("numpy");
        

        // PyRun_SimpleString("print( sys.path )");
        PyObject* clusteringmodule = PyImport_ImportModule("clustering");

        PyObject_Print(clusteringmodule, stdout, 0);
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(clusteringmodule, &pos, &key, &value))
        {
            // int i = PyInt_AS_LONG(value) + 1;
            char* strvalue = PyString_AsString(value);
            char* strkey = PyString_AsString(value);
            printf("%s : %s", strkey, strvalue);
        }


        size_t size = 12;
        // int array[] = { 1,2,3,4,5,6,7,8,9,10,11,12 };
        int* array = new int[12];
        for(int i = 0; i<12;i++){array[i]=3+i;}

        PyObject *mylist = PyList_New(size);
        for (size_t i = 0; i != size; ++i) {
            PyList_SET_ITEM(mylist, i, PyInt_FromLong(array[i]));
        }
        PyObject *arglist = Py_BuildValue("O", mylist);

        PyObject *clusteringCApi = PyObject_GetAttrString( clusteringmodule, "CApi" );
        PyObject* myfunc = PyObject_GetAttrString( clusteringCApi, "call_points_clustering");
        if(myfunc == NULL)
            { return -1; }

        // Py_intptr_t dims = 1;
        import_array1(0);
        npy_intp dims = 1;
        PyObject* numpyList = PyArray_SimpleNewFromData( 12 , &dims,  NPY_INT32, array);
        // PyErr_Print();
        
        PyObject *result = PyObject_CallObject(myfunc, mylist);
        // PyObject_Print(result, stdout, 0);
        int retval = (int)PyInt_AsLong(result);

        Py_DECREF(result);
        Py_DECREF(arglist);
        Py_DECREF(mylist);
        Py_DECREF(myfunc);
        Py_DECREF(clusteringmodule);


        Py_Finalize();
    }

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
