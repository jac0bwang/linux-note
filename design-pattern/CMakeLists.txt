cmake_minimum_required(VERSION 3.8)

project(design-pattern)

set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS
 -g
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 # -Werror
 # -Wconversion
 -Wno-unused-parameter
 # -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 # -std=c++0x
 -rdynamic
 )

if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(${CMAKE_SOURCE_DIR}/strategy)
add_subdirectory(${CMAKE_SOURCE_DIR}/observer)
add_subdirectory(${CMAKE_SOURCE_DIR}/decorator)
