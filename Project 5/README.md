# Project 5: Video Processing using Multiple Threads and SIMD Parallelization

## Setup Instructions (This is for reference right now. This README is far from complete).

Aside from the standard C libraries that come with a standard WSL installation, you will also need OpenCV. Here's how you get it:

Update your package index:
```
sudo apt update
```

Install the required dependencies:
```
sudo apt install build-essential cmake git pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev
```

Clone the OpenCV repository into a desired folder (e.g. the folder with `proj5.c` in it):
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
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
```

Compile and install the library:
```
make -j4
sudo make install
```

Update the shared library cache:
```
sudo ldconfig
```
