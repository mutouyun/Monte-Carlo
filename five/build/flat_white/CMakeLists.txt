project(flat_white)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Quick)
find_package(Qt5QuickCompiler)
qtquick_compiler_add_resources(RESOURCES ${CMAKE_SOURCE_DIR}/../flat_white/qml.qrc)

include_directories(${CMAKE_SOURCE_DIR}/../include)
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/../flat_white/*.cpp)
file(GLOB HEAD_FILES ${CMAKE_SOURCE_DIR}/../flat_white/*.h)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
link_directories(${EXECUTABLE_OUTPUT_PATH})
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEAD_FILES} ${RESOURCES})
qt5_use_modules(${PROJECT_NAME} Core Quick)

target_link_libraries(${PROJECT_NAME} five)
if(NOT MSVC)
  target_link_libraries(${PROJECT_NAME} pthread)
endif()
