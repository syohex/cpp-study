CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT(json_lib)

ENABLE_LANGUAGE(CXX)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_CXX_FLAGS "-Wall -g -fsanitize=address,undefined ${CMAKE_CXX_FLAGS}")

INCLUDE_DIRECTORIES(lib/)

SET(LIB_FILES
        lib/json_array.cpp
        lib/json_object.cpp
        lib/json_boolean.cpp
        lib/json_null.cpp
        lib/json_number.cpp
        lib/json_string.cpp
        lib/json_value.cpp
        lib/json_parser.cpp
        lib/json_exception.cpp lib/json_exception.h)

ADD_LIBRARY(json ${LIB_FILES})

ADD_EXECUTABLE(test test/test.cpp)
TARGET_LINK_LIBRARIES(test json)