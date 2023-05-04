add_library(
    _INFERENCE
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/inference.cpp"
)

target_link_libraries(_INFERENCE 
    PUBLIC ${OpenCV_LIBS}
)

target_include_directories(_INFERENCE 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC ${OpenCV_INCLUDE_DIRS}
)