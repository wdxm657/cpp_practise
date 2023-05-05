# 添加可执行文件
add_executable(test test.cc)

# 链接库文件
target_link_libraries(
    test 
    PUBLIC ${OpenCV_LIBS}
)