add_library(
    _INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.cpp"
)

target_link_libraries(_INTERFACE 
    PUBLIC ${OpenCV_LIBS}
)

target_include_directories(_INTERFACE 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC ${OpenCV_INCLUDE_DIRS}
)