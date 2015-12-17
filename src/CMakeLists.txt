cmake_minimum_required(VERSION 2.6)
project(SpaceInvaders)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14 -O3")

include_directories("${CMAKE_SOURCE_DIR}")

set(SOURCE
    RandomGenerator.cpp
    Scene.cpp
    Stopwatch.cpp
    SpaceInvaders.cpp
)

add_subdirectory(model)
add_subdirectory(view)
add_subdirectory(controller)
add_subdirectory(timeline)
add_subdirectory(parser)
add_subdirectory(tinyxml2)

# add the executable
set(EXECUTABLE_NAME "SpaceInvaders")
add_executable(${EXECUTABLE_NAME} ${SOURCE})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
# Find any version 2.X of SFML
# See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()
