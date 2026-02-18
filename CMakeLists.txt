cmake_minimum_required(VERSION 4.1)
project(TZBot4CPP)

set(CMAKE_CXX_STANDARD 20)

add_library(TZBot4CPP SHARED src/TZBot.cpp
        src/TZRequest.cpp
        src/TZData.cpp
        src/TZResponse.cpp
        src/TZRequestQueue.cpp
        src/encryption/EncryptionFactory.cpp
        src/Exceptions.cpp)

add_library(TZBot4CPP::TZBot4CPP ALIAS TZBot4CPP)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(TZBot4CPP PUBLIC OpenSSL::Crypto ZLIB::ZLIB)
target_include_directories(TZBot4CPP
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
