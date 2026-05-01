cmake_minimum_required(VERSION 3.10)
project(msla_packages_maker)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(msla_packages_maker
    main.cpp
    mslb_parser.cpp
    dll_generator.cpp
    compiler.cpp
)
