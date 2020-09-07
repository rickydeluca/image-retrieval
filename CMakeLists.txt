cmake_minimum_required(VERSION 2.8)
project(image_retrieval)
find_package(OpenCV REQUIRED)
set(img_retrieval_files
    image_retrieval.hpp
    image_retrieval.cpp
    image_retrieval_main.cpp)
add_executable(image_retrieval ${img_retrieval_files})
target_link_libraries(image_retrieval ${OpenCV_LIBS})