cmake_minimum_required(VERSION 2.8.7)
project("Tesis" CXX)

find_package(OpenCV REQUIRED core imgproc ml highgui features2d)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(RaysExtractorMain "cpp/RaysExtractorMain" "header/utility.cpp")
target_link_libraries(RaysExtractorMain ${OpenCV_LIBS})

add_executable(MicroDescriptorBuilderMain "cpp/MicroDescriptorBuilderMain" "header/utility.cpp")
target_link_libraries(MicroDescriptorBuilderMain ${OpenCV_LIBS})

add_executable(MicroDescriptorNormalizeMain "cpp/MicroDescriptorNormalizeMain" "header/utility.cpp")
target_link_libraries(MicroDescriptorNormalizeMain ${OpenCV_LIBS})

add_executable(BoWBuilderMain "cpp/BoWBuilderMain" "header/utility.cpp")
target_link_libraries(BoWBuilderMain ${OpenCV_LIBS})

add_executable(kFoldCrossValidationMain "cpp/kFoldCrossValidationMain" "header/utility.cpp")
target_link_libraries(kFoldCrossValidationMain ${OpenCV_LIBS})

add_executable(kFoldCrossValidationMain_new "cpp/kFoldCrossValidationMain_new" "header/utility.cpp")
target_link_libraries(kFoldCrossValidationMain_new ${OpenCV_LIBS})

add_executable(MacroDescriptorBuilderMain "cpp/MacroDescriptorBuilderMain" "header/utility.cpp")
target_link_libraries(MacroDescriptorBuilderMain ${OpenCV_LIBS})

add_executable(kFoldCreatorFiles "cpp/kFoldCreatorFiles" "header/utility.cpp")
target_link_libraries(kFoldCreatorFiles ${OpenCV_LIBS})

add_executable(TestFileCreator "cpp/TestFileCreator" "header/utility.cpp")
target_link_libraries(TestFileCreator ${OpenCV_LIBS})

add_executable(TesisMain "cpp/TesisMain" "header/utility.cpp")
target_link_libraries(TesisMain ${OpenCV_LIBS})

add_executable(SVMTestValues "cpp/SVMTestValues" "header/utility.cpp")
target_link_libraries(SVMTestValues ${OpenCV_LIBS})

add_executable(VisualRaysTracker "cpp/VisualRaysTracker" "header/utility.cpp")
target_link_libraries(VisualRaysTracker ${OpenCV_LIBS})

add_executable(PaintFaceWithClusters "cpp/PaintFaceWithClusters" "header/utility.cpp")
target_link_libraries(PaintFaceWithClusters ${OpenCV_LIBS})

add_executable(VideoTransformLBP "cpp/VideoTransformLBP" "header/utility.cpp")
target_link_libraries(VideoTransformLBP ${OpenCV_LIBS})

add_executable(RaysErrorCalc "cpp/RaysErrorCalc" "header/utility.cpp")
target_link_libraries(RaysErrorCalc ${OpenCV_LIBS})