add_library(
    _DRIVER
    "${CMAKE_CURRENT_SOURCE_DIR}/config_gui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/dma.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/dma.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/driver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/driver.cpp"
)

target_include_directories(
    _DRIVER 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)