cmake_minimum_required (VERSION 3.18.4)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" ) ## add -v for verbose output
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D _DEBUG ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D _DEBUG ")

project(Space_game VERSION 1.0.0) ### PROJECT

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}           -Wall -Wpedantic -g")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}       -Wall -Wpedantic -g")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Z7 /EHsc /Wall /analyze")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7 /EHsc /Wall /analyze")
endif()



if(WIN32)
endif()

if(UNIX AND NOT APPLE)



        #set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}           -fsanitize=address")
        #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}       -fsanitize=address")
        #set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

endif()




FILE(GLOB MAIN ./main.c)



add_executable(Main ${MAIN})


