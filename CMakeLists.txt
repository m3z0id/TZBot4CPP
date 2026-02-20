cmake_minimum_required(VERSION 4.1)
project(TZBot4CPP)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(SOURCES src/TZBot.cpp
        src/TZRequest.cpp
        src/TZData.cpp
        src/TZResponse.cpp
        src/TZRequestQueue.cpp
        src/Exceptions.cpp
        src/encryption/EncryptionFactory.cpp)

option(TZBOT4CPP_BUILD_SHARED "Build TZBot4CPP as a shared library" ON)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "This library is only compatible with Linux.")
endif()

if(TZBOT4CPP_BUILD_SHARED)
    set(LIB_TYPE SHARED)
    message(STATUS "Building TZBot4CPP as a SHARED library")
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(LIB_TYPE STATIC)
    message(STATUS "Building TZBot4CPP as a STATIC library")
endif()

add_library(TZBot4CPP ${LIB_TYPE} ${SOURCES})
add_library(TZBot4CPP::TZBot4CPP ALIAS TZBot4CPP)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# nlohmann json
if(nlohmann_json_FOUND)
    message(STATUS "Using system's nlohmann/json")
else()
    message(STATUS "nlohmann/json not found. Falling back to amalgamated header...")
    set(JSON_VERSION v3.12.0)
    set(JSON_HEADER_URL https://github.com/nlohmann/json/releases/download/${JSON_VERSION}/json.hpp)
    set(JSON_DEST_DIR "${CMAKE_BINARY_DIR}/external/nlohmann")
    set(JSON_DEST_FILE "${JSON_DEST_DIR}/json.hpp")

    if(NOT EXISTS "${JSON_DEST_FILE}")
        file(MAKE_DIRECTORY "${JSON_DEST_DIR}")
        file(DOWNLOAD "${JSON_HEADER_URL}" "${JSON_DEST_FILE}" SHOW_PROGRESS)
    endif()

    if(NOT TARGET nlohmann_json::nlohmann_json)
        add_library(nlohmann_json INTERFACE)
        add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)

        target_include_directories(nlohmann_json INTERFACE "${CMAKE_BINARY_DIR}/external")
    endif()
endif()

# Linking
target_link_libraries(TZBot4CPP PUBLIC $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>)
target_link_libraries(TZBot4CPP PUBLIC OpenSSL::Crypto ZLIB::ZLIB)
# Public includes
target_include_directories(TZBot4CPP PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)