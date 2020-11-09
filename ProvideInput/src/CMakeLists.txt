add_library(output ProvideOutput.cpp)
add_executable(main main.cpp)



target_link_libraries(main output wsock32 ws2_32)