cmake_minimum_required(VERSION 2.8)

project(pi)

set(CMAKE_CXX_STANDARD 17)
if(NOT MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

add_executable(${PROJECT_NAME} "main.cpp")

if(NOT MSVC)
  target_link_libraries(${PROJECT_NAME} pthread)
endif()
