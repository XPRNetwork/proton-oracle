
cmake_minimum_required(VERSION 3.16)
set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument -DEOSIO_NO_RICARDIAN")

project(atom)

find_package(blanc)
include(EosioWasmToolchain)

add_contract(atom atom
   ${CMAKE_CURRENT_SOURCE_DIR}/src/feed_manager.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/feed_provider.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/feed_msig.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/aggregate.cpp
)

target_include_directories(atom
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)