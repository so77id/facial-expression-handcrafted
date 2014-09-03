cmake_minimum_required(VERSION 2.8.7)
project("Tesis" CXX)

find_package(OpenCV REQUIRED core imgproc ml highgui features2d)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(RaysExtractorMain "cpp/RaysExtractorMain")
target_link_libraries(RaysExtractorMain ${OpenCV_LIBS})

add_executable(MicroDescriptorBuilderMain "cpp/MicroDescriptorBuilderMain")
target_link_libraries(MicroDescriptorBuilderMain ${OpenCV_LIBS})

add_executable(BoWBuilderMain "cpp/BoWBuilderMain")
target_link_libraries(BoWBuilderMain ${OpenCV_LIBS})

add_executable(svmMain "cpp/svmMain")
target_link_libraries(svmMain ${OpenCV_LIBS})

add_executable(kFoldCreatorFiles "cpp/kFoldCreatorFiles")
target_link_libraries(kFoldCreatorFiles ${OpenCV_LIBS})