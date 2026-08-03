# CMakeLists.txt at the project root

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(thingy91x_framework)

# Main application logic
add_subdirectory(src/app)
