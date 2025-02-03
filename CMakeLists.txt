cmake_minimum_required(VERSION 3.29)
project(vci)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
        libavcodec
        libavformat
        libavutil
        libswscale
)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(vci_lib
        src/ascii.cpp
        src/terminal.cpp
        src/video_decoder.cpp

        include/vci/ascii.hpp
        include/vci/terminal.hpp
        include/vci/video_decoder.hpp
)

target_include_directories(vci_lib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(vci_lib PUBLIC
        PkgConfig::FFMPEG
)

add_executable(vci
        src/main.cpp
)

target_link_libraries(vci PRIVATE
        vci_lib
)
