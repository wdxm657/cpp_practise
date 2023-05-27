add_library(
    _WINDOW
    "${CMAKE_CURRENT_SOURCE_DIR}/window.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/window.cc"
)

target_link_libraries(_WINDOW 
  PUBLIC ${GTKMM_LIBRARIES}
  PUBLIC _DRIVER
  PUBLIC pthread
)

target_include_directories(
  _WINDOW 
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
  PUBLIC ${GTKMM_INCLUDE_DIRS}
)