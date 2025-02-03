cmake_minimum_required(VERSION 3.29)
project(vci VERSION 0.1.0)

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

include(GNUInstallDirs)

install(TARGETS vci
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS vci_lib
        EXPORT vci_lib-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/vci
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT vci_lib-targets
        FILE vci-targets.cmake
        NAMESPACE vci::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vci
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        cmake/vci-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/vci-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vci
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/vci-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/vci-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/vci-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vci
)