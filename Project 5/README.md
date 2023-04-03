# Project 5: Video Processing using Multiple Threads and SIMD Parallelization

## Setup Instructions (This is for reference right now. This README is far from complete).

Aside from the standard C libraries that come with a standard WSL installation, you will also need OpenCV. Here's how you get it:

Update your package index:
```
sudo apt update
```

Install the required dependencies:
```
sudo apt-get install build-essential cmake git pkg-config libjpeg8-dev libtiff5-dev libjasper-dev libpng12-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libgtk-3-dev libatlas-base-dev gfortran python3-dev
```

Clone the OpenCV repository into a desired folder (i.e. `cd` to wherever you want to install it before running the `git` command):
```
git clone https://github.com/opencv/opencv.git
```

Create a build directory and navigate to it:
```
cd opencv
mkdir build
cd build
```

Configure the build using CMake:
```
cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    -D OPENCV_ENABLE_NONFREE=ON \
    -D BUILD_EXAMPLES=ON ..
```

Compile and install the library:
```
make -j4
sudo make install
sudo ldconfig
```

Verify the installation:
```
pkg-config --modversion opencv4
```
