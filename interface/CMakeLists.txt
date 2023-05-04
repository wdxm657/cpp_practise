add_library(
    _INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.cpp"
)

target_include_directories(_INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")