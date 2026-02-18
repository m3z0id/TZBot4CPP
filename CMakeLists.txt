cmake_minimum_required(VERSION 4.1)
project(TZBot4CPP)

set(CMAKE_CXX_STANDARD 20)

add_library(TZBot4CPP SHARED src/TZBot.cpp
        src/TZRequest.cpp
        src/TZData.cpp
        src/TZResponse.cpp
        src/Helpers.h
        src/TZRequestQueue.cpp
        src/encryption/EncryptionFactory.cpp
        include/Exceptions.h
        src/Exceptions.cpp)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(TZBot4CPP PUBLIC OpenSSL::Crypto ZLIB::ZLIB)
target_include_directories(TZBot4CPP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
