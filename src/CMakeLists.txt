cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

include (../cmake/FatalWarnings.cmake)
ADM_EXTRA_WARNINGS()

find_package(Protobuf 3 REQUIRED)
find_package(Threads REQUIRED)

# This won't find ZQM in custom locations.
# FIXME(nelson.castillo): Improve.
FIND_LIBRARY(ZMQ_LIB NAMES zmq)

message(STATUS "ZMQ found => " "${ZMQ_LIB}")

PROTOBUF_GENERATE_CPP(MALOS_PROTO_SRC MALOS_PROTO_HEADER ../protocol-buffers/malos/driver.proto)

include_directories(
  ${PROJECT_BINARY_DIR} # Protos.
  ${PROTOBUF_INCLUDE_DIRS})

set(zmq_wrapper_lib_sources
  zmq_push.h
  zmq_push.cpp
  zmq_pull.h
  zmq_pull.cpp
)

add_library(zmq_wrapper_lib ${zmq_wrapper_lib_sources})
set_property(TARGET zmq_wrapper_lib PROPERTY CXX_STANDARD 11)
target_link_libraries(zmq_wrapper_lib ${ZMQ_LIB} ${CMAKE_THREAD_LIBS_INIT})

set(malos_src
  malos_base.cpp malos_base.h
  tcp_client.cpp tcp_client.h
  ${MALOS_PROTO_SRC} ${MALOS_PROTO_HEADER}
)

add_library(malos ${malos_src})
set_property(TARGET malos PROPERTY CXX_STANDARD 11)
target_link_libraries(malos zmq_wrapper_lib)
target_link_libraries(malos ${PROTOBUF_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(malos wiringPi)

# Binary to be installed.
install(TARGETS malos DESTINATION lib)
