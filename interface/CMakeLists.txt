add_library(
    interface
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.cpp"
)

target_include_directories(interface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")