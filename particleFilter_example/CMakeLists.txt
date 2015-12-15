cmake_minimum_required(VERSION 2.8)
project( particleFilter_example )
#for some reason "-Wl,--no-as-needed" compiler flags are needed (not sure, but i may be a bug in gcc)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wl,--no-as-needed")
find_package( OpenCV REQUIRED )
find_package( Threads )
add_executable( particleFilter_example 
particleFilter_example.cpp
particleFilter.cpp)
target_link_libraries( particleFilter_example ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})

