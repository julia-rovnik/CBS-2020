cmake_minimum_required(VERSION 2.8)
project(try-multirobot)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(SOURCE_FILES
        agent.cpp
        agent.h
        agents.cpp
        agents.h
        constr.h
        ctNode.cpp
        ctNode.h
        main.cpp
        map.cpp
        map.h
        search.cpp
        search.h
        searchNode.h
        )
add_executable(try-multirobot ${SOURCE_FILES})