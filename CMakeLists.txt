cmake_minimum_required(VERSION 3.16)
project(Project05)
aux_source_directory(./src DIR_SRC)
add_executable(mat ${DIR_SRC} src/data.hpp src/tidy.cpp)