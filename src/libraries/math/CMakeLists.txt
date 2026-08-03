# src/libraries/math/CMakeLists.txt

zephyr_library()
zephyr_library_sources(
    matrix.c
    quaternion.c
)
zephyr_library_include_directories(.)
