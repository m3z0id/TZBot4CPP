cmake_minimum_required(VERSION 4.1)
project(TZBot4CPP)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(SOURCES src/TZBot.cpp
        src/TZRequest.cpp
        src/TZData.cpp
        src/TZResponse.cpp
        src/TZRequestQueue.cpp
        src/encryption/EncryptionFactory.cpp)

option(TZBOT4CPP_BUILD_SHARED "Build TZBot4CPP as a shared library" ON)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "This library is only compatible with Linux.")
endif()

if(TZBOT4CPP_BUILD_SHARED)
    set(LIB_TYPE SHARED)
    message(STATUS "Building TZBot4CPP as a SHARED library")
else()
    set(LIB_TYPE STATIC)
    message(STATUS "Building TZBot4CPP as a STATIC library")
endif()

add_library(TZBot4CPP ${LIB_TYPE} ${SOURCES})
add_library(TZBot4CPP::TZBot4CPP ALIAS TZBot4CPP)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# nlohmann json
if(NOT TARGET nlohmann_json::nlohmann_json)
    FetchContent_Declare(
            json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.12.0
    )
    FetchContent_MakeAvailable(json)

    if(TARGET nlohmann_json AND NOT TARGET nlohmann_json::nlohmann_json)
        add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
    endif()
endif ()

# Linking
target_link_libraries(TZBot4CPP PUBLIC $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>)
target_link_libraries(TZBot4CPP PUBLIC OpenSSL::Crypto ZLIB::ZLIB)
# Public includes
target_include_directories(TZBot4CPP PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)