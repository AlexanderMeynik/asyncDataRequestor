cmake_minimum_required(VERSION 3.26)
project(postgessTest)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE " -O3 -march=native")
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_package(cpr REQUIRED)
find_package(pugixml REQUIRED)


find_package(OpenMP REQUIRED)
find_package(quill REQUIRED)

add_executable(runRequestor main.cpp databaseService.cpp databaseService.h
        resultsStruct.cpp
        resultsStruct.h
        resultsStruct.h)
target_link_libraries(runRequestor PRIVATE cpr::cpr ${PQXX_LIB} ${PQ_LIB} pugixml::pugixml OpenMP::OpenMP_CXX  quill::quill)

