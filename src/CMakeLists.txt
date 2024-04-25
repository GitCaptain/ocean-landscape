cmake_minimum_required(VERSION 3.16)

project(LandscapeGenerator VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILES
    main.cpp
    generator.cpp
    generator.h
    vox_writer.cpp
    vox_writer.h
    logger.h
    common.h
    utils.cpp
    utils.h
    measure.h
    measure.cpp
    noise/FastNoiseLite.h
    noise.h
    noise.cpp
    )

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DEBUG)
endif()
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
