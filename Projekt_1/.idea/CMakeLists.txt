cmake_minimum_required(VERSION 3.29)
project(_idea)

set(CMAKE_CXX_STANDARD 14)

add_executable(_idea
    .gitignore
    modules.xml
    Projekt_1.iml
    vcs.xml
        ../main.cpp)
