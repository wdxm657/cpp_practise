add_library(
    _DRIVER
    "${CMAKE_CURRENT_SOURCE_DIR}/driver.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/driver.cpp"
)

target_include_directories(_DRIVER PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")