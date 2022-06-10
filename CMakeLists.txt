cmake_minimum_required (VERSION "3.14.2")

if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
    FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
    ")
endif()

project(Subprocess)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sanitizer Build 
if ("${SANITIZE}" STREQUAL "address")
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif ()

######################################
##### BUILD SUBPROCESS LIBRARY #######
######################################
file(GLOB source_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(subprocess STATIC ${source_files})

###########################################
##### BUILD SUBPROCESS TEST EXECUTABLE ####
###########################################
add_executable(test_subprocess ${CMAKE_CURRENT_SOURCE_DIR}/test/subprocess_test.cc)

target_link_libraries(test_subprocess subprocess)
