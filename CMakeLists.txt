cmake_minimum_required(VERSION 3.3)
project(main LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES("include")
INCLUDE_DIRECTORIES("include/ICP")
INCLUDE_DIRECTORIES("include/utils")


FILE(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB ICP "${PROJECT_SOURCE_DIR}/src/ICP/*.cpp")
FILE(GLOB UTILS "${PROJECT_SOURCE_DIR}/src/utils/*.cpp")

add_executable(main ${SOURCES} ${ICP} ${UTILS})

target_link_libraries(main)