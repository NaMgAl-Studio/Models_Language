cmake_minimum_required(VERSION 3.10)
project(msla_interpreter)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(msla_interpreter 
    main.cpp
    lexer.cpp
    parser.cpp
    interpreter.cpp
    error.cpp
)
