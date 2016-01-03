cmake_minimum_required(VERSION 2.8)
project( RMVIS3_project )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wl,--no-as-needed") #enable c++11
find_package( OpenCV REQUIRED )
find_package( Threads )
add_executable( RMVIS3_project
src/main.cpp
src/particleFilter.cpp
src/point3dReconstruction.cpp
src/imgProc.hpp
src/imgProc.cpp)
target_link_libraries( RMVIS3_project ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})

