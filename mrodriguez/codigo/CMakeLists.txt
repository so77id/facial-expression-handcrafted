cmake_minimum_required(VERSION 2.8.7)
project("Tesis" CXX)

find_package(OpenCV REQUIRED core imgproc highgui features2d)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(RaysExtractorMain "RaysExtractorMain")
target_link_libraries(RaysExtractorMain ${OpenCV_LIBS})

add_executable(MicroDescriptorBuilderMain "MicroDescriptorBuilderMain")
target_link_libraries(MicroDescriptorBuilderMain ${OpenCV_LIBS})

add_executable(BoWBuilderMain "BoWBuilderMain")
target_link_libraries(BoWBuilderMain ${OpenCV_LIBS})
