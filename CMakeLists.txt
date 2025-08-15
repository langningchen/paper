cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(paper CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

find_library(NPCAP_WPCAP_LIBRARY NAMES wpcap PATHS ${CMAKE_SOURCE_DIR}/libs/x64)
find_library(NPCAP_PACKET_LIBRARY NAMES Packet PATHS ${CMAKE_SOURCE_DIR}/libs/x64)

message(STATUS "Found Npcap wpcap lib: ${NPCAP_WPCAP_LIBRARY}")
message(STATUS "Found Npcap Packet lib: ${NPCAP_PACKET_LIBRARY}")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/npcap)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ws2_32
        wininet
        iphlpapi
        urlmon
        ${NPCAP_PACKET_LIBRARY}
        ${NPCAP_WPCAP_LIBRARY}
        -static-libgcc
        -static-libstdc++
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ws2_32
        wininet
        iphlpapi
        urlmon
        ${NPCAP_PACKET_LIBRARY}
        ${NPCAP_WPCAP_LIBRARY}
    )
endif()
