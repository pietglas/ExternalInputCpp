add_library(input ProvideOutput.cpp)
add_executable(main_input main.cpp)



target_link_libraries(main_input input wsock32 ws2_32)