cmake_minimum_required(VERSION 2.8)
project( stereo_capture )
#for some reason "-Wl,--no-as-needed" compiler flags are needed (not sure, but i may be a bug in gcc)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wl,--no-as-needed")
find_package( OpenCV REQUIRED )
find_package( Threads )
add_executable( stereo_capture stereo_capture.cpp)
target_link_libraries( stereo_capture ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})

