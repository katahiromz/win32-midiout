# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.6)

# project name, version, and languages
project(midiout VERSION 0.0 LANGUAGES CXX)

##############################################################################

# midiout1.exe
add_executable(midiout1 midiout1.cpp)
target_link_libraries(midiout1 PRIVATE winmm)

# midiout2.exe
add_executable(midiout2 midiout2.cpp)
target_link_libraries(midiout2 PRIVATE winmm)

# play_mid.exe
add_executable(play_mid play_mid.cpp)
target_link_libraries(play_mid PRIVATE winmm)

##############################################################################
