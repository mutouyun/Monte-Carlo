project(five)

add_compile_options(-D__FIVE_LIBRARY__)

include_directories(${CMAKE_SOURCE_DIR}/../include ${CMAKE_SOURCE_DIR}/../src)
aux_source_directory(${CMAKE_SOURCE_DIR}/../src SRC_FILES)
file(GLOB HEAD_FILES ${CMAKE_SOURCE_DIR}/../include/*.h ${CMAKE_SOURCE_DIR}/../src/*.h)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${HEAD_FILES})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
