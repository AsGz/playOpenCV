

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

find_package( OpenCV REQUIRED )

add_executable(canny_detector canny_detector.cpp)
target_link_libraries(canny_detector ${OpenCV_LIBS})

add_executable(canny_cut canny_cut.cpp)
target_link_libraries(canny_cut ${OpenCV_LIBS})






