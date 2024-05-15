cmake_minimum_required(VERSION 3.5)

project(CSSOptimizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CSSOptimizer CSSOptimizer.cpp CSSOptimizer.h main.cpp)
add_library(CSSOptimizerShared SHARED CSSOptimizer.cpp CSSOptimizer.h)
target_compile_definitions(CSSOptimizerShared PRIVATE SHARED_LIB=1)
set_target_properties(CSSOptimizerShared PROPERTIES OUTPUT_NAME "CSSOptimizer")
add_library(CSSOptimizerStatic STATIC CSSOptimizer.cpp CSSOptimizer.h)

include(GNUInstallDirs)
install(TARGETS CSSOptimizer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
