

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

find_package( OpenCV REQUIRED )

add_executable(canny_detector canny_detector.cpp)
target_link_libraries(canny_detector ${OpenCV_LIBS})

add_executable(canny_cut canny_cut.cpp)
target_link_libraries(canny_cut ${OpenCV_LIBS})

add_executable(drawing drawing.cpp)
target_link_libraries(drawing ${OpenCV_LIBS})

add_executable(histograms histograms.cpp)
target_link_libraries(histograms ${OpenCV_LIBS})

add_executable(histogram_calculation histogram_calculation.cpp)
target_link_libraries(histogram_calculation ${OpenCV_LIBS})

add_executable(histogram_equalization histogram_equalization.cpp)
target_link_libraries(histogram_equalization ${OpenCV_LIBS})

add_executable(histograms_matching histograms_matching.cpp)
target_link_libraries(histograms_matching ${OpenCV_LIBS})

add_executable(mat_add mat_add.cpp)
target_link_libraries(mat_add ${OpenCV_LIBS})

add_executable(sobel_test sobel_test.cpp)
target_link_libraries(sobel_test ${OpenCV_LIBS})

add_executable(quad-segmentation quad-segmentation.cpp)
target_link_libraries(quad-segmentation ${OpenCV_LIBS})

add_executable(hough_lines hough_lines.cpp)
target_link_libraries(hough_lines ${OpenCV_LIBS})

add_executable(stitching_detail stitching_detail.cpp)
target_link_libraries(stitching_detail ${OpenCV_LIBS})

#add templte
#add_executable(newadd newadd.cpp)
#target_link_libraries(newadd ${OpenCV_LIBS})






