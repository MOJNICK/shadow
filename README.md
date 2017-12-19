# shadow
packages:

apt-get install build-essential
apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
apt-get install libav-tools

installation:

cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local -D PYTHON2_EXECUTABLE=/usr/bin/python2.7 -D PYTHON_INCLUDE_DIR=/usr/include/python2.7/ -D PYTHON_INCLUDE_DIR2=/usr/include/x86_64-linux-gnu/python2.7 -D PYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython2.7.so -D PYTHON2_NUMPY_INCLUDE_DIRS=/usr/lib/python2.7/dist-packages/numpy/core/include/ -D WITH_FFMPEG=ON ..

cd shadow
cmake ./libcomparator -DWITH_TESTS=ON -DTEST_PRIVATE_PART=ON

./libcomparator/test/execute/execute 
./libcomparator/test/unit/testcomparator
