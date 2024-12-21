cmake_minimum_required(VERSION 3.16)

project(genetic_algo_revisited LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(genetic_algo_revisited main.cpp
    individual.h individual.cpp
    individualfactory.h individualfactory.cpp
    population.h population.cpp
    geneticalgo.h geneticalgo.cpp)

include(GNUInstallDirs)
install(TARGETS genetic_algo_revisited
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
