cmake_minimum_required(VERSION 3.3)
project(main LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(OpenCV REQUIRED)

message(STATUS "version: ${OpenCV_VERSION}")
message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
message(STATUS "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")

INCLUDE_DIRECTORIES("include")
INCLUDE_DIRECTORIES("include/ICP")
INCLUDE_DIRECTORIES("include/utils")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include/Opengl_Render")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/glad/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/glfw-3.3.6/include")


FILE(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB ICP "${PROJECT_SOURCE_DIR}/src/ICP/*.cpp")
FILE(GLOB UTILS "${PROJECT_SOURCE_DIR}/src/utils/*.cpp")
FILE(GLOB Opengl_Render "${PROJECT_SOURCE_DIR}/src/Opengl_Render/*.cpp")
FILE(GLOB GLAD "${PROJECT_SOURCE_DIR}/external/glad/src/*.cpp")

link_directories(
    "${PROJECT_SOURCE_DIR}/external/glfw-3.3.6/bin"
)

add_executable(main ${SOURCES} 
                    ${ICP} 
                    ${UTILS}
                    ${Opengl_Render}
                    ${GLAD}
                    )

target_link_libraries(main 
                    glfw3
                    ${OpenCV_LIBS}
                    )